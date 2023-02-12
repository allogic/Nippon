#pragma once

#include <string>
#include <sstream>
#include <filesystem>
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
  class StringUtils
  {
  public:

    static std::string CutFront(const std::string& String, U64 Size);
    static std::string CutBack(const std::string& String, U64 Size);
    static std::string RemoveNulls(const std::string& String);
    static std::string ToLower(std::string String);
    static std::string ToUpper(std::string String);
    static std::string PosixPath(const std::string& String);
    static std::string SelectExpr(const std::string& String, const std::string& Expr);
  };
}