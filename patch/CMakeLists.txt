file (GLOB_RECURSE SRC_FILES "./src/*.c" "./src/*.cpp")

add_executable (patch ${SRC_FILES})

target_compile_definitions (patch PRIVATE _CRT_SECURE_NO_WARNINGS)

target_include_directories (patch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries (patch PRIVATE core)

target_precompile_headers (patch PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/np_patch_pch.hpp")