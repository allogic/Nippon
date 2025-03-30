#if !defined(NP_CORE_STRING_HPP)
#  define NP_CORE_STRING_HPP

class NpString : public std::string {
public:
  NpString &CutFront(uint64_t const Size);
  NpString &CutBack(uint64_t const Size);
  NpString &RemoveChars(char const Char);
  NpString &ToLower();
  NpString &ToUpper();
  NpString &Select(NpString const &Expression);

  NpString CutFront(uint64_t const Size) const;
  NpString CutBack(uint64_t const Size) const;
  NpString RemoveChars(char const Char) const;
  NpString ToLower() const;
  NpString ToUpper() const;
  NpString Select(NpString const &Expression) const;

public:
  static NpString ByteToHexStr(uint8_t const Byte);
  static uint8_t HexStrToByte(NpString const &String);
};

#endif
