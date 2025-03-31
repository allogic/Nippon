FetchContent_Declare (
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG        "docking"
)

set (IMGUI_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-src")

FetchContent_MakeAvailable (imgui)
