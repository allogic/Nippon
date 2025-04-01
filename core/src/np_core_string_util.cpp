#include <np_core_pch.hpp>
#include <np_core_string_util.hpp>

std::string NpStringUtil::CutFront(std::string const &String, uint64_t Size) {
  return String.substr(Size);
}
std::string NpStringUtil::CutBack(std::string const &String, uint64_t Size) {
  return String.substr(0, String.size() - Size);
}
std::string NpStringUtil::RemoveChars(std::string const &String, char Char) {
  std::string Result = String;
  Result.erase(std::remove_if(Result.begin(), Result.end(), [Char](char const C) { return C == Char; }), Result.end());
  return Result;
}
std::string NpStringUtil::ToLower(std::string const &String) {
  std::string Result = String;
  std::transform(Result.begin(), Result.end(), Result.begin(), [](char const C) { return std::tolower(C); });
  return Result;
}
std::string NpStringUtil::ToUpper(std::string const &String) {
  std::string Result = String;
  std::transform(Result.begin(), Result.end(), Result.begin(), [](char const C) { return std::toupper(C); });
  return Result;
}
std::string NpStringUtil::SelectBy(std::string const &String, std::string const &Expression) {
  std::stringstream Stream;
  for (uint64_t StringIndex = 0; StringIndex < String.size(); StringIndex++) {
    if (StringIndex + Expression.size() <= String.size()) {
      for (uint64_t ExpressionIndex = 0; ExpressionIndex < Expression.size(); ExpressionIndex++) {
        if (Expression[ExpressionIndex] == 'X') {
          Stream << String[StringIndex + ExpressionIndex];
        }
      }

      break;
    }
  }
  return Stream.str();
}
