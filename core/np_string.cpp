#include <np_pch.hpp>
#include <np_string.hpp>

NpString const &NpString::CutFront(uint64_t const Size) const {
  *this = substr(Size);
  return *this;
}
NpString &NpString::CutFront(uint64_t const Size) {
  *this = substr(Size);
  return *this;
}
void NpString::CutFront(uint64_t const Size) { *this = substr(Size); }

NbString const &NpString::CutBack(uint64_t const Size) {
  *this = substr(0, size() - Size);
  return *this;
}
NbString &NpString::CutBack(uint64_t const Size) {
  *this = substr(0, size() - Size);
  return *this;
}
void NpString::CutBack(uint64_t const Size) { *this = substr(0, size() - Size); }

NpString const &NpString::RemoveChars(char const Char) {
  std::string Result = String;
  Result.erase(std::remove_if(Result.begin(), Result.end(), [=](char const C) { return C == Char; }), Result.end());
  return Result;
}

std::string NpString::ToLower(std::string const &String) {
  std::string Result = String;
  std::transform(Result.begin(), Result.end(), Result.begin(), [](char const C) { return std::tolower(C); });
  return Result;
}
std::string NpString::ToUpper(std::string const &String) {
  std::string Result = String;
  std::transform(Result.begin(), Result.end(), Result.begin(), [](char const C) { return std::toupper(C); });
  return Result;
}

std::string NpString::SelectExpr(std::string const &String, std::string const &Expression) {
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

std::string NpString::ByteToHexStr(int8_t const Byte) {
  char String[3] = {};
  std::sprintf(String, "%02x", Byte);
  return String;
}
uint8_t NpString::HexStrToByte(std::string const &String) { return (uint8_t)std::strtol(String.data(), nullptr, 16); }
