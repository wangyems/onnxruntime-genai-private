set(GENERATORS_ROOT ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
add_subdirectory(models)
file(GLOB generator_srcs CONFIGURE_DEPENDS
   "${GENERATORS_ROOT}/*.h"
   "${GENERATORS_ROOT}/*.cpp"
   "${MODELS_ROOT}/*.h"
   "${MODELS_ROOT}/*.cpp"
)
if(USE_CUDA AND CMAKE_CUDA_COMPILER)
  # Don't let cmake set a default value for CMAKE_CUDA_ARCHITECTURES
  cmake_policy(SET CMP0104 OLD)
  enable_language(CUDA)
  message(STATUS "CMAKE_CUDA_COMPILER_VERSION: ${CMAKE_CUDA_COMPILER_VERSION}")
  # set(CUDA_PROPAGATE_HOST_FLAGS ON)
  set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcudafe --diag_suppress=2803 --expt-relaxed-constexpr")
  file(GLOB generator_cuda_srcs CONFIGURE_DEPENDS
     "${GENERATORS_ROOT}/*.cu"
     "${GENERATORS_ROOT}/*.cuh"
     "${MODELS_ROOT}/*.cu"
     "${MODELS_ROOT}/*.cuh"
  )
  list(APPEND generator_srcs ${generator_cuda_srcs})
  add_compile_definitions(USE_CUDA=1)
  include_directories("${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")
else()
  file(GLOB generator_cuda_srcs "${GENERATORS_ROOT}/*_cuda*.*")
  list(REMOVE_ITEM generator_srcs ${generator_cuda_srcs})
endif()

add_library(onnxruntime-genai ${generator_srcs})
target_include_directories(onnxruntime-genai PRIVATE ${ORT_HEADERS_DIR})