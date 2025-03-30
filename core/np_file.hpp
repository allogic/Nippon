#if !defined(NP_CORE_FILE_HPP)
#  define NP_CORE_FILE_HPP

#  include <np_path.hpp>

class NpFile {
public:
  static void ReadBinary(NpPath const &Path, std::vector<uint8_t> &Bytes);
  static void ReadText(NpPath const &Path, std::vector<uint8_t> &Bytes);

  static void WriteBinary(NpPath const &Path, std::vector<uint8_t> const &Bytes);
  static void WriteBinary(NpPath const &Path, uint8_t const *Bytes, uint64_t Size);

  static void WriteText(NpPath const &Path, std::vector<uint8_t> const &Bytes);
  static void WriteText(NpPath const &Path, uint8_t const *Bytes, uint64_t Size);
};

#endif
