if (ANDROID)
    add_compile_definitions(USE_CXX17=1)
    message("Using C++17")
    set(CMAKE_CXX_STANDARD 17)
else ()
    message("Using C++20")
    set(CMAKE_CXX_STANDARD 20)
endif ()