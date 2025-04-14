file (GLOB_RECURSE SRC_FILES "./src/*.c" "./src/*.cpp")

add_executable (editor ${SRC_FILES} ${IMGUI_SRC_FILES})

target_compile_definitions (editor PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions (editor PRIVATE NP_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/shader\")

target_include_directories (editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories (editor PRIVATE ${GLFW_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${GLM_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${IMGUI_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${ENTT_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${SQLITECPP_INCLUDE_DIR})
target_include_directories (editor PRIVATE ${DIRECTXTEX_INCLUDE_DIR})

target_link_libraries (editor PRIVATE Vulkan::Vulkan)
target_link_libraries (editor PRIVATE core)
target_link_libraries (editor PRIVATE glfw)
target_link_libraries (editor PRIVATE EnTT::EnTT)
target_link_libraries (editor PRIVATE SQLiteCpp)
target_link_libraries (editor PRIVATE DirectXTex)

target_precompile_headers (editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/np_editor_pch.hpp")

add_custom_command (TARGET editor
  POST_BUILD

  COMMAND ${CUSTOM_OUTPUT_DIR}/bin/patch.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe .dovs ${CMAKE_SOURCE_DIR}/shader/np_shader_default_object.vert.spv ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe
  COMMAND ${CMAKE_COMMAND} -E rename ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe

  COMMAND ${CUSTOM_OUTPUT_DIR}/bin/patch.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe .dofs ${CMAKE_SOURCE_DIR}/shader/np_shader_default_object.frag.spv ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe
  COMMAND ${CMAKE_COMMAND} -E rename ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe

  COMMAND ${CUSTOM_OUTPUT_DIR}/bin/patch.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe .dlvs ${CMAKE_SOURCE_DIR}/shader/np_shader_debug_line.vert.spv ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe
  COMMAND ${CMAKE_COMMAND} -E rename ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe

  COMMAND ${CUSTOM_OUTPUT_DIR}/bin/patch.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe .dofs ${CMAKE_SOURCE_DIR}/shader/np_shader_default_object.frag.spv ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe
  COMMAND ${CMAKE_COMMAND} -E rename ${CUSTOM_OUTPUT_DIR}/bin/editor_patched.exe ${CUSTOM_OUTPUT_DIR}/bin/editor.exe
)
