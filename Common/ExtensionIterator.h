#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class ExtensionIterator
  {
  public:

    ExtensionIterator(const fs::path& File, const std::string& Ext);

  public:

    inline auto begin() { return mFiles.begin(); }
    inline const auto begin() const { return mFiles.cbegin(); }

    inline auto end() { return mFiles.end(); }
    inline const auto end() const { return mFiles.cend(); }

  private:

    std::vector<fs::path> mFiles;
  };
}