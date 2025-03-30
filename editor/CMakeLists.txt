file (GLOB_RECURSE SRC_FILES "*.c" "*.cpp")

add_executable (editor ${SRC_FILES})

target_compile_definitions (editor PRIVATE "_CRT_SECURE_NO_WARNINGS")

target_include_directories (editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (editor PRIVATE ${GLFW_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${IMGUI_INCLUDE_DIR})

target_link_libraries (editor PRIVATE glfw)
target_link_libraries (editor PRIVATE Vulkan::Vulkan)

target_precompile_headers (editor PRIVATE "np_pch.hpp")