#include <np_pch.hpp>
#include <np_string.hpp>

NpString &NpString::CutFront(uint64_t const Size) {
  *this = substr(Size);
  return *this;
}
NpString &NpString::CutBack(uint64_t const Size) {
  *this = substr(0, size() - Size);
  return *this;
}
NpString &NpString::RemoveChars(char const Char) {
  erase(std::remove_if(begin(), end(), [Char](char const C) { return C == Char; }), end());
  return *this;
}
NpString &NpString::ToLower() {
  std::transform(begin(), end(), begin(), [](char const C) { return std::tolower(C); });
  return *this;
}
NpString &NpString::ToUpper() {
  std::transform(begin(), end(), begin(), [](char const C) { return std::toupper(C); });
  return *this;
}
NpString &NpString::SelectExpr(std::string const &Expression) {
  std::stringstream Stream;

  for (uint64_t StringIndex = 0; StringIndex < size(); StringIndex++) {
    if (StringIndex + Expression.size() <= size()) {
      for (uint64_t ExpressionIndex = 0; ExpressionIndex < Expression.size(); ExpressionIndex++) {
        if (Expression[ExpressionIndex] == 'X') {
          Stream << this[StringIndex + ExpressionIndex];
        }
      }

      break;
    }
  }

  *this = Stream.str();

  return *this;
}

NpString NpString::CutFront(uint64_t const Size) const { return substr(Size); }
NpString NpString::CutBack(uint64_t const Size) const { return substr(0, size() - Size); }
NpString NpString::RemoveChars(char const Char) const {
  NpString String = *this;
  String.erase(std::remove_if(String.begin(), String.end(), [Char](char const C) { return C == Char; }), String.end());
  return String;
}
NpString NpString::ToLower() const {
  NpString String = *this;
  std::transform(String.begin(), String.end(), String.begin(), [](char const C) { return std::tolower(C); });
  return String;
}
NpString NpString::ToUpper() const {
  NpString String = *this;
  std::transform(String.begin(), String.end(), String.begin(), [](char const C) { return std::toupper(C); });
  return String;
}
NpString NpString::SelectExpr(std::string const &Expression) const {
  std::stringstream Stream;

  for (uint64_t StringIndex = 0; StringIndex < size(); StringIndex++) {
    if (StringIndex + Expression.size() <= size()) {
      for (uint64_t ExpressionIndex = 0; ExpressionIndex < Expression.size(); ExpressionIndex++) {
        if (Expression[ExpressionIndex] == 'X') {
          Stream << this[StringIndex + ExpressionIndex];
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
