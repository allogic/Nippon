#include <np_editor_pch.hpp>
#include <np_editor.hpp>
#include <np_editor_context.hpp>

#define NP_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME ".dovs"
#define NP_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME ".dofs"
#define NP_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME ".dlvs"
#define NP_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME ".dlfs"

#if defined(OS_WINDOWS)
// #  pragma comment(linker, "/INCLUDE:g_DefaultObjectVertexShader") // TODO
// #  pragma comment(linker, "/INCLUDE:g_DefaultObjectFragmentShader") // TODO
// #  pragma comment(linker, "/INCLUDE:g_DebugLineVertexShader") // TODO
// #  pragma comment(linker, "/INCLUDE:g_DebugLineFragmentShader") // TODO

#  pragma section(NP_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME, read)
#  pragma section(NP_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME, read)
#  pragma section(NP_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME, read)
#  pragma section(NP_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME, read)

__declspec(allocate(NP_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME)) static char const *g_DefaultObjectVertexShader;
__declspec(allocate(NP_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME)) static char const *g_DefaultObjectFragmentShader;
__declspec(allocate(NP_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME)) static char const *g_DebugLineVertexShader;
__declspec(allocate(NP_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME)) static char const *g_DebugLineFragmentShader;
#else defined(OS_LINUX)
__attribute__((section(NP_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME))) static char const *g_DefaultObjectVertexShader;
__attribute__((section(NP_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME))) static char const *g_DefaultObjectFragmentShader;
__attribute__((section(NP_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME))) static char const *g_DebugLineVertexShader;
__attribute__((section(NP_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME))) static char const *g_DebugLineFragmentShader;
#endif

int main(int Argc, char **Argv, char **Envp) {
  if (g_Context.Create(1920, 1080)) {
    g_Context.Run();
    g_Context.Destroy();
  }

  return 0;
}