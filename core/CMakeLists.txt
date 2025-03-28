file (GLOB_RECURSE SRC_FILES "*.c" "*.cpp")

add_library (core ${SRC_FILES})

target_compile_definitions (core PRIVATE "_CRT_SECURE_NO_WARNINGS")

target_precompile_headers (core PRIVATE "pch.hpp")