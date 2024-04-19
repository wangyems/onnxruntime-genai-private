#pragma once

// TODO (pavignol): Refactor

#include <wrl/client.h>
#include <wil/result.h>
#include <d3d12.h>
#include <DirectML.h>
#include "dml_execution_context.h"

using Microsoft::WRL::ComPtr;

struct DmlReusedCommandListState {
  // Re-usable command list, supporting descriptor heap, and DML binding table to update that heap.
  Microsoft::WRL::ComPtr<IDMLCompiledOperator> compiled_operator;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphics_command_list;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
  Microsoft::WRL::ComPtr<IDMLBindingTable> binding_table;
  Microsoft::WRL::ComPtr<ID3D12Resource> persistent_resource;
  OrtValue* previousOutput = nullptr;
};

struct DmlObjects {
  ComPtr<ID3D12Device> d3d12_device;
  ComPtr<ID3D12CommandQueue> command_queue;
  ComPtr<ID3D12CommandAllocator> command_allocator;
  ComPtr<ID3D12GraphicsCommandList> command_list;
  ComPtr<ID3D12Resource> upload_buffer;
};

inline DmlObjects CreateDmlObjects() {
  D3D12_COMMAND_QUEUE_DESC command_queue_description = {
      D3D12_COMMAND_LIST_TYPE_COMPUTE,
      0,
      D3D12_COMMAND_QUEUE_FLAG_NONE,
      0,
  };

  DmlObjects dml_objects;

  THROW_IF_FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dml_objects.d3d12_device)));
  THROW_IF_FAILED(dml_objects.d3d12_device->CreateCommandQueue(&command_queue_description, IID_PPV_ARGS(&dml_objects.command_queue)));
  THROW_IF_FAILED(dml_objects.d3d12_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&dml_objects.command_allocator)));
  THROW_IF_FAILED(dml_objects.d3d12_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dml_objects.command_allocator.Get(), nullptr, IID_PPV_ARGS(&dml_objects.command_list)));
  return dml_objects;
}

inline DmlReusedCommandListState BuildReusableCommandList(
    IDMLDevice* dml_device,
    IDMLCompiledOperator* compiled_operator,
    ID3D12Resource* persistent_resource,
    std::optional<DML_BUFFER_BINDING> persistent_resource_binding) {
  DmlReusedCommandListState command_list_state{};

  DML_BINDING_PROPERTIES exec_binding_props = compiled_operator->GetBindingProperties();

  D3D12_DESCRIPTOR_HEAP_DESC desc = {};
  desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  desc.NumDescriptors = exec_binding_props.RequiredDescriptorCount;
  desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

  ComPtr<ID3D12Device> d3d_device;
  THROW_IF_FAILED(dml_device->GetParentDevice(IID_PPV_ARGS(&d3d_device)));

  THROW_IF_FAILED(d3d_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(command_list_state.heap.ReleaseAndGetAddressOf())));

  // Create a binding table for execution.
  DML_BINDING_TABLE_DESC binding_table_desc = {};
  binding_table_desc.Dispatchable = compiled_operator;
  binding_table_desc.CPUDescriptorHandle = command_list_state.heap->GetCPUDescriptorHandleForHeapStart();
  binding_table_desc.GPUDescriptorHandle = command_list_state.heap->GetGPUDescriptorHandleForHeapStart();
  binding_table_desc.SizeInDescriptors = exec_binding_props.RequiredDescriptorCount;

  THROW_IF_FAILED(dml_device->CreateBindingTable(&binding_table_desc, IID_PPV_ARGS(&command_list_state.binding_table)));

  THROW_IF_FAILED(d3d_device->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_COMPUTE,
      IID_PPV_ARGS(command_list_state.command_allocator.ReleaseAndGetAddressOf())));

  THROW_IF_FAILED(d3d_device->CreateCommandList(
      0,
      D3D12_COMMAND_LIST_TYPE_COMPUTE,
      command_list_state.command_allocator.Get(),
      nullptr,
      IID_PPV_ARGS(command_list_state.graphics_command_list.ReleaseAndGetAddressOf())));

  if (persistent_resource) {
    DML_BINDING_DESC persistent_resource_binding_desc = {DML_BINDING_TYPE_BUFFER, persistent_resource_binding ? &*persistent_resource_binding : nullptr};
    command_list_state.binding_table->BindPersistentResource(&persistent_resource_binding_desc);
    command_list_state.persistent_resource = persistent_resource;
  }

  ID3D12DescriptorHeap* descriptor_heaps[] = {command_list_state.heap.Get()};
  command_list_state.graphics_command_list->SetDescriptorHeaps(ARRAYSIZE(descriptor_heaps), descriptor_heaps);

  ComPtr<IDMLCommandRecorder> recorder;
  THROW_IF_FAILED(dml_device->CreateCommandRecorder(IID_PPV_ARGS(recorder.GetAddressOf())));

  recorder->RecordDispatch(command_list_state.graphics_command_list.Get(), compiled_operator, command_list_state.binding_table.Get());
  command_list_state.compiled_operator = compiled_operator;

  THROW_IF_FAILED(command_list_state.graphics_command_list->Close());

  return command_list_state;
}

inline void ExecuteReusableCommandList(
    DmlExecutionContext* execution_context,
    DmlReusedCommandListState& command_list_state,
    OrtAllocator& allocator,
    const OrtDmlApi* ort_dml_api,
    std::span<ID3D12Resource*> input_resources,
    std::span<const uint64_t> input_sizes,
    std::span<ID3D12Resource*> output_resources,
    std::span<const uint64_t> output_sizes,
    bool bindings_changed) {
  assert(input_resources.size() == input_sizes.size());
  assert(output_resources.size() == output_sizes.size());

  DML_BINDING_PROPERTIES exec_binding_props = command_list_state.compiled_operator->GetBindingProperties();

  std::vector<DML_BUFFER_BINDING> input_bindings(input_resources.size());
  std::vector<DML_BINDING_DESC> input_binding_descs(output_resources.size());

  std::vector<DML_BUFFER_BINDING> output_bindings(output_resources.size());
  std::vector<DML_BINDING_DESC> output_binding_descs(output_resources.size());

  if (bindings_changed) {
    // Bind the inputs
    for (uint32_t i = 0; i < input_bindings.size(); ++i) {
      input_bindings[i].Buffer = input_resources[i];
      input_bindings[i].SizeInBytes = input_sizes[i];
      input_binding_descs[i] = {DML_BINDING_TYPE_BUFFER, &input_bindings[i]};
    }

    command_list_state.binding_table->BindInputs(static_cast<uint32_t>(input_binding_descs.size()), input_binding_descs.data());

    // Bind the outputs
    for (uint32_t i = 0; i < output_bindings.size(); ++i) {
      output_bindings[i].Buffer = output_resources[i];
      output_bindings[i].SizeInBytes = output_sizes[i];
      output_binding_descs[i] = {DML_BINDING_TYPE_BUFFER, &output_bindings[i]};
    }

    command_list_state.binding_table->BindOutputs(static_cast<uint32_t>(output_binding_descs.size()), output_binding_descs.data());

    // Create the temporary resource
    if (exec_binding_props.TemporaryResourceSize > 0) {
      ComPtr<ID3D12Resource> temporary_resource;
      std::array<int64_t, 1> persistent_resource_shape = {static_cast<int64_t>(exec_binding_props.TemporaryResourceSize)};
      auto persistent_tensor = OrtValue::CreateTensor(allocator, persistent_resource_shape, ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8);
      Ort::ThrowOnError(ort_dml_api->GetD3D12ResourceFromAllocation(&allocator, persistent_tensor->GetTensorMutableRawData(), &temporary_resource));
    }
  }

  // Execute the command list and if it succeeds, update the fence value at which this command may be
  // re-used.
  ComPtr<ID3D12Fence> fence;
  uint64_t completion_value;
  execution_context->ExecuteCommandList(command_list_state.graphics_command_list.Get(), fence.GetAddressOf(), &completion_value);
}

inline ComPtr<IDMLCompiledOperator> CreateCastOperator(
    IDMLDevice* dml_device,
    uint32_t num_elements,
    DML_TENSOR_DATA_TYPE source_data_type,
    DML_TENSOR_DATA_TYPE target_data_type) {
  // Create the input tensor desc
  DML_BUFFER_TENSOR_DESC input_buffer_desc{};
  input_buffer_desc.Sizes = &num_elements;
  input_buffer_desc.DimensionCount = 1;
  input_buffer_desc.DataType = source_data_type;

  switch (source_data_type) {
    case DML_TENSOR_DATA_TYPE_FLOAT16:
      input_buffer_desc.TotalTensorSizeInBytes = num_elements * sizeof(Ort::Float16_t);
      break;
    case DML_TENSOR_DATA_TYPE_FLOAT32:
      input_buffer_desc.TotalTensorSizeInBytes = num_elements * sizeof(float);
      break;
    default:
      THROW_HR(E_NOTIMPL);
  }

  DML_TENSOR_DESC input_tensor_desc = {DML_TENSOR_TYPE_BUFFER, &input_buffer_desc};

  // Create the output tensor desc
  DML_BUFFER_TENSOR_DESC output_buffer_desc{};
  output_buffer_desc.Sizes = &num_elements;
  output_buffer_desc.DimensionCount = 1;
  output_buffer_desc.DataType = target_data_type;

  switch (target_data_type) {
    case DML_TENSOR_DATA_TYPE_FLOAT16:
      output_buffer_desc.TotalTensorSizeInBytes = num_elements * sizeof(Ort::Float16_t);
      break;
    case DML_TENSOR_DATA_TYPE_FLOAT32:
      output_buffer_desc.TotalTensorSizeInBytes = num_elements * sizeof(float);
      break;
    default:
      THROW_HR(E_NOTIMPL);
  }

  DML_TENSOR_DESC output_tensor_desc = {DML_TENSOR_TYPE_BUFFER, &output_buffer_desc};

  DML_CAST_OPERATOR_DESC cast_op_desc{};
  cast_op_desc.InputTensor = &input_tensor_desc;
  cast_op_desc.OutputTensor = &output_tensor_desc;
  DML_OPERATOR_DESC cast_op_dml_desc = {DML_OPERATOR_CAST, &cast_op_desc};

  ComPtr<IDMLOperator> cast_op;
  THROW_IF_FAILED(dml_device->CreateOperator(&cast_op_dml_desc, IID_PPV_ARGS(&cast_op)));

  ComPtr<IDMLCompiledOperator> compiled_cast_op;
  THROW_IF_FAILED(dml_device->CompileOperator(cast_op.Get(), DML_EXECUTION_FLAG_DESCRIPTORS_VOLATILE, IID_PPV_ARGS(&compiled_cast_op)));

  return compiled_cast_op;
}

inline void GetNextDispatchSize(
    uint32_t element_count,
    uint32_t num_threads,
    uint32_t& dispatch,
    uint32_t& pending_element_count) {
  // Max threads per workgroup is 2^10 (1024). Max dispatch per dimension is 2^16. Taken together, we can dispatch a maximum of
  // 2^26 (268,435,456) threads along a single dimension. This should suffice for a majority of the workload. Therefore, even
  // though it is possible to dispatch up to (2^16)^3 workgroups simultaneously, we stick to the simpler 1D dispatch alternative.
  assert(num_threads <= D3D12_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP);

  const uint32_t max_threads_per_dispatch = num_threads * D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;

  // Compute max dispatchable elements
  const uint32_t available_thread_count = std::min(element_count, max_threads_per_dispatch);

  // Compute required thread group count
  uint32_t workgroup_count_1d = (available_thread_count + num_threads - 1) / num_threads;

  // Compute min dispatch size
  dispatch = workgroup_count_1d;

  // With the dispatch size computed, compute the dispatched element count
  const uint32_t dispatched_element_count = workgroup_count_1d * num_threads;

  // Update the pending element count
  pending_element_count = (dispatched_element_count < element_count) ? element_count - dispatched_element_count : 0;
}

inline DML_TENSOR_DATA_TYPE OrtToDmlDataType(ONNXTensorElementDataType ort_dtype) {
  switch (ort_dtype) {
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
      return DML_TENSOR_DATA_TYPE_FLOAT16;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
      return DML_TENSOR_DATA_TYPE_FLOAT32;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:
      return DML_TENSOR_DATA_TYPE_FLOAT64;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
      return DML_TENSOR_DATA_TYPE_UINT8;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
      return DML_TENSOR_DATA_TYPE_UINT16;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32:
      return DML_TENSOR_DATA_TYPE_UINT32;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
      return DML_TENSOR_DATA_TYPE_UINT64;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
      return DML_TENSOR_DATA_TYPE_INT8;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
      return DML_TENSOR_DATA_TYPE_INT16;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
      return DML_TENSOR_DATA_TYPE_INT32;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
      return DML_TENSOR_DATA_TYPE_INT64;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
      return DML_TENSOR_DATA_TYPE_UINT8;
    default:
      THROW_HR(E_NOTIMPL);
  }
}

inline void DmlCastInputToOutput(
    DmlExecutionContext* execution_context,
    OrtAllocator& allocator,
    OrtValue& in,
    std::unique_ptr<OrtValue>& p_out,
    IDMLDevice* dml_device,
    const OrtDmlApi* ort_dml_api,
    DmlReusedCommandListState& command_list_state) {
  auto shape_info = in.GetTensorTypeAndShapeInfo();
  auto shape = shape_info->GetShape();
  assert(shape_info->GetElementType() == Ort::TypeToTensorType<Ort::Float16_t>::type);

  bool allocate_p_out = p_out == nullptr;
  if (p_out) {
    auto out_shape_info = p_out->GetTensorTypeAndShapeInfo();
    auto out_shape = out_shape_info->GetShape();
    allocate_p_out = shape != out_shape;
  }

  if (allocate_p_out) {
    p_out = OrtValue::CreateTensor<float>(allocator, shape);
  }

  int element_count = static_cast<int>(shape_info->GetElementCount());

  bool rebind = command_list_state.previousOutput != p_out.get();

  // If the sizes change, we need to recompile the operator and rebuild the command lists. It should only happen
  // once after the very first iteration.
  if (rebind) {
    auto dml_from_type = OrtToDmlDataType(in.GetTensorTypeAndShapeInfo()->GetElementType());
    auto dml_to_type = OrtToDmlDataType(p_out->GetTensorTypeAndShapeInfo()->GetElementType());
    auto compiled_cast_operator = CreateCastOperator(dml_device, element_count, dml_from_type, dml_to_type);

    ComPtr<ID3D12Resource> persistent_resource;
    uint64_t persistent_resource_size = compiled_cast_operator->GetBindingProperties().PersistentResourceSize;

    std::optional<DML_BUFFER_BINDING> persistent_resource_binding;

    if (persistent_resource_size > 0) {
      std::array<int64_t, 1> persistent_resource_shape = {static_cast<int64_t>(persistent_resource_size)};
      auto persistent_tensor = OrtValue::CreateTensor(allocator, persistent_resource_shape, ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8);
      Ort::ThrowOnError(ort_dml_api->GetD3D12ResourceFromAllocation(&allocator, persistent_tensor->GetTensorMutableRawData(), &persistent_resource));
      persistent_resource_binding = DML_BUFFER_BINDING{persistent_resource.Get(), 0, persistent_resource_size};
    }

    DML_BINDING_DESC persistent_resource_bindingDesc = persistent_resource_binding
                                                           ? DML_BINDING_DESC{DML_BINDING_TYPE_BUFFER, &*persistent_resource_binding}
                                                           : DML_BINDING_DESC{DML_BINDING_TYPE_NONE, nullptr};

    DML_BINDING_DESC input_array_binding_desc = DML_BINDING_DESC{DML_BINDING_TYPE_NONE, nullptr};
    execution_context->InitializeOperator(compiled_cast_operator.Get(), persistent_resource_bindingDesc, input_array_binding_desc);
    command_list_state = BuildReusableCommandList(dml_device, compiled_cast_operator.Get(), persistent_resource.Get(), persistent_resource_binding);
    command_list_state.previousOutput = p_out.get();
  }

  ComPtr<ID3D12Resource> source_resource;
  Ort::ThrowOnError(ort_dml_api->GetD3D12ResourceFromAllocation(&allocator, in.GetTensorMutableData<uint8_t>(), &source_resource));

  ComPtr<ID3D12Resource> target_resource;
  Ort::ThrowOnError(ort_dml_api->GetD3D12ResourceFromAllocation(&allocator, p_out->GetTensorMutableData<uint8_t>(), &target_resource));

  std::array<ID3D12Resource*, 1> input_resources = {source_resource.Get()};
  std::array<uint64_t, 1> input_sizes = {element_count * sizeof(Ort::Float16_t)};

  std::array<ID3D12Resource*, 1> output_resources = {target_resource.Get()};
  ;
  std::array<uint64_t, 1> output_sizes = {element_count * sizeof(float)};

  ExecuteReusableCommandList(execution_context, command_list_state, allocator, ort_dml_api, input_resources, input_sizes, output_resources, output_sizes, rebind);
}
