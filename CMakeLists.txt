cmake_minimum_required (VERSION 3.21)

project (Nippon LANGUAGES C CXX)

find_package (Vulkan REQUIRED)

set (CMAKE_C_STANDARD 23)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

# set (VULKAN_SDK_DIR "/opt/vulkansdk/1.4.309.0/x86_64")

include (FetchContent)

include ("${CMAKE_SOURCE_DIR}/cmake/build_targets.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/compiler_config.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/platform_selection.cmake")

include ("${CMAKE_SOURCE_DIR}/cmake/fetch_glfw.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/fetch_imgui.cmake")

add_subdirectory (core)
add_subdirectory (editor)