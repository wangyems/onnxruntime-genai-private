if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 10)
    add_compile_definitions(USE_CXX17=1)
    message("Test is using C++17 because GCC Version is less than 10")
    set(CMAKE_CXX_STANDARD 17)
elseif (USE_CUDA AND CMAKE_CUDA_COMPILER AND CMAKE_CUDA_COMPILER_VERSION VERSION_LESS 12)
    add_compile_definitions(USE_CXX17=1)
    message("Test is using C++17 Because CUDA Version is less than 12")
    set(CMAKE_CXX_STANDARD 17)
else ()
    message("Test is using C++20")
    set(CMAKE_CXX_STANDARD 20)
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 9)
    add_compile_definitions(USE_EXPERIMENTAL_FILESYSTEM)
endif()