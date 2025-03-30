#if !defined(NP_CORE_STRING_HPP)
#  define NP_CORE_STRING_HPP

class NpString : public std::string {
public:
  NpString const &CutFront(uint64_t const Size) const;
  NpString &CutFront(uint64_t const Size);
  void CutFront(uint64_t const Size);

  NpString const &CutBack(uint64_t const Size) const;
  NpString &CutBack(uint64_t const Size);
  void CutBack(uint64_t const Size);

  NpString RemoveChars(char const Char);
  NpString ToLower();
  NpString ToUpper();
  NpString SelectExpr(NpString const &Expression);

public:
  static NpString ByteToHexStr(uint8_t const Byte);
  static uint8_t HexStrToByte(NpString const &String);
};

#endif
