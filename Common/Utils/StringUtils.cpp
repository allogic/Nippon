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

  std::string StringUtils::PosixPath(const std::string& String)
  {
    std::string result = String;
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
  }

  std::string StringUtils::SelectExpr(const std::string& String, const std::string& Expr)
  {
    std::stringstream stream;

    for (U32 i = 0; i < String.size(); i++)
    {
      if (i + Expr.size() <= String.size())
      {
        for (U32 j = 0; j < Expr.size(); j++)
        {
          if (Expr[j] == 'X')
          {
            stream << String[i + j];
          }
        }

        break;
      }
    }

    return stream.str();
  }
}