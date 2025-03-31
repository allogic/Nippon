set (ENTT_INCLUDE_NATVIS ON)

FetchContent_Declare (
  entt
  GIT_REPOSITORY "https://github.com/skypjack/entt.git"
  GIT_TAG        "v3.15.0"
)

set (ENTT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/entt-src/signle_include")

FetchContent_MakeAvailable (entt)
