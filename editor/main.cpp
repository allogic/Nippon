#include <pch.hpp>
#include <editor.hpp>

int main() {
  NpEditor Editor;

  if (Editor.Create(1920, 1080)) {
    Editor.Destroy();
  }

  return 0;
}