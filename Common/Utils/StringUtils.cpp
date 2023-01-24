#include <Common/Utils/StringUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::string StringUtils::CutFront(const std::string& String, U64 Size)
  {
    return String.substr(Size);
  }

  std::string StringUtils::CutBack(const std::string& String, U64 Size)
  {
    return String.substr(0, String.size() - Size);
  }

  std::string StringUtils::RemoveNulls(const std::string& String)
  {
    std::stringstream stream;

    for (U32 i = 0; i < String.size(); i++)
    {
      if (String[i] != 0)
      {
        stream << String[i];
      }
    }

    return stream.str();
  }

  std::string StringUtils::NormalizePath(const std::string& String)
  {
    return fs::weakly_canonical(String).make_preferred().string();
  }
}