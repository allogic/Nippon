file (GLOB_RECURSE SRC_FILES "./src/*.c" "./src/*.cpp")

add_library (core ${SRC_FILES})

target_compile_definitions (core PRIVATE "_CRT_SECURE_NO_WARNINGS")

target_include_directories (core PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_precompile_headers (core PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/np_core_pch.hpp")