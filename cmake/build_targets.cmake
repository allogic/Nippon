if (NOT CMAKE_BUILD_TYPE)

  set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose a build type (Debug, Release)" FORCE)

endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")

  add_compile_definitions (BUILD_DEBUG)

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")

  

else ()

  message (FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")

endif ()