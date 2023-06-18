#pragma once

#include <vector>
#include <filesystem>

#include <Common/Types.h>

#include <Editor/Header.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class ItsSerializer
  {
  public:

    static std::vector<ItsEntry> FromFile(const fs::path& File);
    static void ToFile(const fs::path& File, const std::vector<ItsEntry>& Entries);
  };
}