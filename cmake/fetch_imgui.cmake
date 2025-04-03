FetchContent_Declare (
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG        "docking"
)

set (IMGUI_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-src")

file (GLOB IMGUI_SRC_FILES "${IMGUI_INCLUDE_DIR}/*.cpp" "${IMGUI_INCLUDE_DIR}/backends/imgui_impl_glfw.cpp" "${IMGUI_INCLUDE_DIR}/backends/imgui_impl_vulkan.cpp")
