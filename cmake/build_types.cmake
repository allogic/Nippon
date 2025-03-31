if (CMAKE_CONFIGURATION_TYPES)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES MATCHES "Debug")

        add_compile_definitions (BUILD_DEBUG)

    elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES MATCHES "Release")

    else ()

      message (FATAL_ERROR "Unknown build type")

    endif ()

endif ()
