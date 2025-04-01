option (LIEF_DOC OFF)
option (LIEF_PYTHON_API OFF)
option (LIEF_EXAMPLES OFF)
option (LIEF_TESTS OFF)

FetchContent_Declare (
  lief
  GIT_REPOSITORY "https://github.com/lief-project/LIEF.git"
  GIT_TAG        "0.16.4"
)

set (GLM_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/lief-src/include")

FetchContent_MakeAvailable (lief)
