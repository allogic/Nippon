#if !defined(NP_CORE_FILE_UTIL_HPP)
#  define NP_CORE_FILE_UTIL_HPP

#  include <np_core_path_util.hpp>

class NpFileUtil {
public:
  static void ReadBinary(std::filesystem::path const &File, std::vector<uint8_t> &Bytes);
  static void ReadText(std::filesystem::path const &File, std::vector<uint8_t> &Bytes);

  static void WriteBinary(std::filesystem::path const &File, std::vector<uint8_t> const &Bytes);
  static void WriteBinary(std::filesystem::path const &File, uint8_t const *Bytes, uint64_t Size);

  static void WriteText(std::filesystem::path const &File, std::vector<uint8_t> const &Bytes);
  static void WriteText(std::filesystem::path const &File, uint8_t const *Bytes, uint64_t Size);
};

#endif
