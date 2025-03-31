#include <np_editor_pch.hpp>
#include <np_editor.hpp>
#include <np_editor_context.hpp>

int main(int Argc, char **Argv, char **Envp) {
  if (g_Context.Create(1920, 1080)) {
    g_Context.Run();
    g_Context.Destroy();
  }

  return 0;
}