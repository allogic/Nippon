cmake_minimum_required(VERSION 3.8)

include(Compiler)
include(Platform)

set(OpenGL_GL_PREFERENCE GLVND)
set(TARGET_NAME Editor)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE CRC_SOURCE ${VENDOR_DIR}/CRC/*.c ${VENDOR_DIR}/CRC/*.cpp)
file(GLOB_RECURSE DDS_SOURCE ${VENDOR_DIR}/DDS/*.c ${VENDOR_DIR}/DDS/*.cpp)
file(GLOB_RECURSE GLAD_SOURCE ${VENDOR_DIR}/GLAD/*.c ${VENDOR_DIR}/GLAD/*.cpp)
file(GLOB_RECURSE IMGUI_SOURCE ${VENDOR_DIR}/ImGui/*.c ${VENDOR_DIR}/ImGui/*.cpp)
file(GLOB_RECURSE TARGET_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${TARGET_NAME}
  ${CRC_SOURCE}
  ${DDS_SOURCE}
  ${GLAD_SOURCE}
  ${IMGUI_SOURCE}
  ${TARGET_SOURCE}
)

set_target_properties(${TARGET_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${BINARY_DIR}
  LIBRARY_OUTPUT_DIRECTORY ${BINARY_DIR}
  RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}
  PREFIX ""
  C_STANDARD 11
  CXX_STANDARD 23
)

target_include_directories(${TARGET_NAME}
  PUBLIC ${ROOT_DIR}
)

target_link_libraries(${TARGET_NAME}
  PUBLIC ${OPENGL_LIBRARIES}
  PUBLIC ${CMAKE_DL_LIBS}
  PUBLIC ${BINARY_DIR}/Common${STATIC_LIBRARY_EXT}
  PUBLIC ${LIBRARY_DIR}/${CMAKE_SYSTEM_NAME}/glfw3${STATIC_LIBRARY_EXT}
)