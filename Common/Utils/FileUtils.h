#pragma once

#include <vector>
#include <fstream>
#include <algorithm>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class FileUtils
  {
  public:

    static std::vector<U8> ReadBinary(const std::string& File, U64 Size = 0);
    static std::string ReadText(const std::string& File, U64 Size = 0);

    static void WriteBinary(const std::string& File, const std::vector<U8>& Bytes, U64 Size = 0);
    static void WriteText(const std::string& File, const std::string& Text, U64 Size = 0);
  };
}