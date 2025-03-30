#include <np_pch.hpp>
#include <np_context.hpp>

int main() {
  NpContext Context;

  if (Context.Create(1920, 1080)) {
    Context.Destroy();
  }

  return 0;
}