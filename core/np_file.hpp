#if !defined(NP_CORE_FILE_HPP)
#  define NP_CORE_FILE_HPP

class NpFile {
public:
  static void ReadBinary(fs::path const &FilePath, std::vector<uint8_t> &Bytes);
  static void ReadText(fs::path const &FilePath, std::vector<uint8_t> &Bytes);

  static void WriteBinary(fs::path const &FilePath, std::vector<uint8_t> const &Bytes);
  static void WriteBinary(fs::path const &FilePath, uint8_t const *Bytes, uint64_t Size);

  static void WriteText(fs::path const &FilePath, std::vector<uint8_t> const &Bytes);
  static void WriteText(fs::path const &FilePath, uint8_t const *Bytes, uint64_t Size);

  static void CreateDirIfNotExist(fs::path const &FilePath, bool DotIsDirectory = false);

  static std::vector<uint64_t> FindStringsInFile(std::vector<uint8_t> const &Bytes, std::vector<uint8_t> const &Pattern);
};

#endif
