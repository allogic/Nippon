#cmake_policy (SET CMP0077 NEW)

option (GLM_TEST_ENABLE OFF)
option (GLM_TEST_ENABLE_CXX_20 OFF)
option (GLM_TEST_ENABLE_SIMD OFF)
option (GLM_TEST_ENABLE_FAST OFF)
option (GLM_STATIC_LIBRARY OFF)
option (GLM_BUILD_DOCS OFF)
option (GLM_BUILD_TESTS OFF)

FetchContent_Declare (
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm.git"
  GIT_TAG        "master"
)

set (GLM_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/glm-src")

FetchContent_MakeAvailable (glm)
