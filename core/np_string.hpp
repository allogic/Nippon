#if !defined(NP_CORE_STRING_HPP)
#  define NP_CORE_STRING_HPP

class NpString : public std::string {
public:
  NpString();
  NpString(NpString const &Other);
  NpString(char const *Other);
  NpString(std::string const &Other);
  NpString(std::filesystem::path const &Other);

public:
  NpString &operator=(NpString const &Other);
  NpString &operator=(char const *Other);
  NpString &operator=(std::string const &Other);
  NpString &operator=(std::filesystem::path const &Other);

public:
  NpString &CutFront(uint64_t const Size);
  NpString &CutBack(uint64_t const Size);
  NpString &RemoveChars(char const Char);
  NpString &ToLower();
  NpString &ToUpper();
  NpString &SelectBy(NpString const &Expression);

  NpString CutFront(uint64_t const Size) const;
  NpString CutBack(uint64_t const Size) const;
  NpString RemoveChars(char const Char) const;
  NpString ToLower() const;
  NpString ToUpper() const;
  NpString SelectBy(NpString const &Expression) const;
};

#endif
