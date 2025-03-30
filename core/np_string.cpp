#include <np_pch.hpp>
#include <np_string.hpp>

NpString::NpString() : std::string() {}
NpString::NpString(NpString const &Other) : std::string(Other) {}
NpString::NpString(char const *Other) : std::string(Other) {}
NpString::NpString(std::string const &Other) : std::string(Other) {}
NpString::NpString(std::filesystem::path const &Other) : std::string(Other.string()) {}

NpString &NpString::operator=(NpString const &Other) {
  std::string::operator=(Other);
  return *this;
}
NpString &NpString::operator=(char const *Other) {
  std::string::operator=(Other);
  return *this;
}
NpString &NpString::operator=(std::string const &Other) {
  std::string::operator=(Other);
  return *this;
}
NpString &NpString::operator=(std::filesystem::path const &Other) {
  std::string::operator=(Other.string());
  return *this;
}

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
NpString &NpString::SelectBy(NpString const &Expression) {
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
NpString NpString::SelectBy(NpString const &Expression) const {
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
