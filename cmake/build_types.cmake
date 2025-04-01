message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "CONFIG: $<CONFIG>")

if ("$<CONFIG:Debug>" STREQUAL "Debug")

  add_compile_definitions (BUILD_DEBUG)

elseif ("$<CONFIG:Release>" STREQUAL "Release")



else ()

  message (FATAL_ERROR "Unknown build type")

endif ()
