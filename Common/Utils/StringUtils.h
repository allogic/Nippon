#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <format>
#include <cmath>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class StringUtils
  {
  public:

    static std::string RemoveNulls(const std::string& String);

    static std::string DumpAsHex(U64 Count, U16 Split, const std::vector<U8>& Bytes);
    static std::string DumpAsU32(U32 Count, const std::vector<U8>& Bytes);

    static U32 Crc32(const std::string& String, U64 DefaultChunkSize = (4ULL * 1024ULL));
    static U32 Crc32(const std::vector<U8>& Bytes, U64 DefaultChunkSize = (4ULL * 1024ULL));
  };
}