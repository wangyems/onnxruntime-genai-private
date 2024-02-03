set(USE_CUDA 1) # "Build with CUDA support"
set(USE_TOKENIZER 1) # "Build with Tokenizer support"
set(BUILD_WHEEL 1)
# Checking if CUDA is supported
include(CheckLanguage)
check_language(CUDA)
set(CMAKE_CUDA_COMPILER ${CMAKE_CUDA_COMPILER} PARENT_SCOPE)

if(WIN32)
  set(ONNXRUNTIME_LIB "onnxruntime.lib" PARENT_SCOPE)
  set(ONNXRUNTIME_FILES "onnxruntime*.dll" PARENT_SCOPE)
  set(ONNXRUNTIME_EXTENSIONS_LIB "tfmtok_c.lib" PARENT_SCOPE)
  set(ONNXRUNTIME_EXTENSIONS_FILES "tfmtok_c.dll" PARENT_SCOPE)
elseif(APPLE)
  set(ONNXRUNTIME_LIB "libonnxruntime.dylib" PARENT_SCOPE)
  set(ONNXRUNTIME_FILES "libonnxruntime*.dylib" PARENT_SCOPE)
else()
  set(ONNXRUNTIME_LIB "libonnxruntime.so" PARENT_SCOPE)
  set(ONNXRUNTIME_FILES "libonnxruntime*.so*" PARENT_SCOPE)
  set(ONNXRUNTIME_EXTENSIONS_LIB "tfmtok_c.so" PARENT_SCOPE)
endif()