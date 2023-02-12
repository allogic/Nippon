cmake_minimum_required(VERSION 3.8)

project(Nippon VERSION 0.0.1 LANGUAGES C CXX)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Cmake)
set(BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Binary)
set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Common)
set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Library)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_DIR})

add_subdirectory(${ROOT_DIR}/Common)
add_subdirectory(${ROOT_DIR}/Editor)

add_subdirectory(${ROOT_DIR}/Tools/BinaryAnalyzer)