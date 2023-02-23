#pragma once

#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

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
  class FileUtils
  {
  public:

    static std::vector<U8> ReadBinary(const fs::path& File, U64 Size = 0);
    static std::string ReadText(const fs::path& File, U64 Size = 0);

    static void WriteBinary(const fs::path& File, const std::vector<U8>& Bytes, U64 Size = 0);
    static void WriteText(const fs::path& File, const std::string& Text, U64 Size = 0);
  };
}