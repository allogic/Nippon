cmake_minimum_required (VERSION 3.21)

include (FetchContent)

project (Nippon LANGUAGES C CXX)

find_package (Vulkan REQUIRED)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CUSTOM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output")

foreach (OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})

  string (TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG_UPPER)

  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER} "${CUSTOM_OUTPUT_DIR}/bin")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER} "${CUSTOM_OUTPUT_DIR}/lib")
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER} "${CUSTOM_OUTPUT_DIR}/lib")

endforeach ()

add_compile_definitions (BUILD_DEBUG=$<IF:$<CONFIG:Debug>,1,0>)

include ("${CMAKE_SOURCE_DIR}/cmake/project_version.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/compiler_config.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/platform_selection.cmake")

include ("${CMAKE_SOURCE_DIR}/cmake/fetch_glfw.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/fetch_glm.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/fetch_imgui.cmake")
include ("${CMAKE_SOURCE_DIR}/cmake/fetch_entt.cmake")

add_subdirectory (core)
add_subdirectory (patch)
add_subdirectory (editor)