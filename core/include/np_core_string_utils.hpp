#if !defined(NP_CORE_STRING_UTILS_HPP)
#  define NP_CORE_STRING_UTILS_HPP

class NpStringUtils {
public:
  static std::string CutFront(std::string const &String, uint64_t Size);
  static std::string CutBack(std::string const &String, uint64_t Size);
  static std::string RemoveChars(std::string const &String, char Char);
  static std::string ToLower(std::string const &String);
  static std::string ToUpper(std::string const &String);
  static std::string SelectBy(std::string const &String, std::string const &Expression);
};

#endif
