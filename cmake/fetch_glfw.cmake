# sudo apt install pkg-config
# sudo apt install wayland-protocols libwayland-dev
# sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
# sudo apt install libxkbcommon-dev
# sudo apt install libvulkan-dev
# sudo apt install glslang-tools

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_BUILD_DOCS OFF)
option (GLFW_BUILD_WAYLAND ON)
option (GLFW_BUILD_X11 OFF)
option (GLFW_BUILD_SHARED OFF)
option (GLFW_INSTALL OFF)

FetchContent_Declare (
  glfw
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG        "3.4"
)

set (GLFW_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/glfw-src/include")

FetchContent_MakeAvailable (glfw)
