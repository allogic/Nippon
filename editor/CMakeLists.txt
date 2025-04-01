file (GLOB_RECURSE SRC_FILES "./src/*.c" "./src/*.cpp")

add_executable (editor ${SRC_FILES})

target_compile_definitions (editor PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions (editor PRIVATE NP_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/shader\")

target_include_directories (editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories (editor PRIVATE ${GLFW_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${GLM_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${IMGUI_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${ENTT_INCLUDE_DIR})

target_link_libraries (editor PRIVATE core)
target_link_libraries (editor PRIVATE glfw)
target_link_libraries (editor PRIVATE glm)
target_link_libraries (editor PRIVATE EnTT::EnTT)
target_link_libraries (editor PRIVATE Vulkan::Vulkan)

target_precompile_headers (editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/np_editor_pch.hpp")
