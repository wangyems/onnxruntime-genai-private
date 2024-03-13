#pragma once

namespace Generators {

struct StaticBuffer {
  // Add max_beam_batch_size to the constructor
  StaticBuffer(Ort::Allocator* allocator);
  ~StaticBuffer();

  std::unique_ptr<OrtValue> GetOrCreateTensor(std::span<const int64_t> shape,
                                              ONNXTensorElementDataType type);

 private:
  size_t GetElementSize(ONNXTensorElementDataType type);

  Ort::Allocator* allocator_{nullptr};
  const OrtMemoryInfo& info_;
  void* buffer_{nullptr};
  size_t bytes_{0};
};

}  // namespace Generators