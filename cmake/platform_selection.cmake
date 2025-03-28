if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

  add_compile_definitions (OS_WINDOWS)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

  add_compile_definitions (OS_LINUX)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")

  add_compile_definitions (OS_DARWIN)

else ()

  message (FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}")

endif ()