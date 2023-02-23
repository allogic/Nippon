#pragma once

#include <cstdlib>
#include <string>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class ArchiveUtils
  {
  public:

    static void FromArchiveName(const std::string& String, U16& Index, std::string& Name, std::string& Type);
    static std::string ToArchiveName(U16 Index, const std::string& Name, const std::string& Type);
  };
}