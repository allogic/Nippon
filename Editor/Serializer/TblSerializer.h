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
  class TblSerializer
  {
  public:

    static std::vector<ObjEntry> FromFile(const fs::path& File);
    static void ToFile(const fs::path& File, const std::vector<ObjEntry>& Entries);
  };
}