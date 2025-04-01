execute_process (
  COMMAND git rev-parse --short HEAD
  OUTPUT_VARIABLE GIT_VERSION_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_compile_definitions (NP_VERSION_MAJOR=\"1\")
add_compile_definitions (NP_VERSION_MINOR=\"1\")
add_compile_definitions (NP_VERSION_PATCH=\"0\")
add_compile_definitions (GIT_VERSION_HASH=\"${GIT_VERSION_HASH}\")
