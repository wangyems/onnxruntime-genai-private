if(USE_ROCM AND NOT EXISTS "${ORT_LIB_DIR}/${ONNXRUNTIME_PROVIDERS_ROCM_LIB}")
  message(FATAL_ERROR "Expected the ONNX Runtime providers ROCm library to be found at ${ORT_LIB_DIR}/${ONNXRUNTIME_PROVIDERS_ROCM_LIB}. Actual: Not found.")
endif()

if(USE_ROCM)
  list(APPEND onnxruntime_libs "${ORT_LIB_DIR}/${ONNXRUNTIME_PROVIDERS_ROCM_LIB}")
  add_compile_definitions(USE_ROCM=1)
endif()