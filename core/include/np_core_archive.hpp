#if !defined(NP_CORE_ARCHIVE_HPP)
#  define NP_CORE_ARCHIVE_HPP

#  include <np_core_binary_processor.hpp>

class NpArchive {
public:
  NpArchive();
  NpArchive(NpArchive *Parent);
  virtual ~NpArchive();

  inline auto const &IsDirectory() const { return m_IsDirectory; }
  inline auto const &GetBytes() const { return m_Bytes; }
  inline auto const &GetSize() const { return m_Size; }
  inline auto const &GetParent() const { return m_Parent; }
  inline auto const &GetType() const { return m_Type; }
  inline auto const &GetName() const { return m_Name; }
  inline auto const &GetParentOffset() const { return m_ParentOffset; }
  inline auto const &GetParentIndex() const { return m_ParentIndex; }

  void SetData(uint8_t const *Bytes, uint64_t Size);

  void Serialize(std::vector<uint8_t> &Bytes);
  void Deserialize(uint8_t const *Bytes, uint64_t Size);
  void Deserialize(std::vector<uint8_t> const &Bytes);

  void ExtractToDisk(std::filesystem::path const &File);
  void UnfoldToDisk(std::filesystem::path const &File);
  void PrintTableOfContent(std::function<void(char const *)> Callback, uint32_t Offset = 0, uint32_t Indent = 0, uint32_t Increment = 2);
  void PrintOfType(std::function<void(char const *)> Callback, std::string const &Type);
  NpArchive *FindArchiveByType(std::string const &Type);
  NpArchive *FindArchiveByName(std::string const &Name);
  NpArchive *FindArchiveByTypeAndName(std::string const &Type, std::string const &Name);
  std::vector<NpArchive *> FindArchivesByType(std::string const &Type);
  std::vector<NpArchive *> FindArchivesByName(std::string const &Name);

private:
  void SerializeRecursive();
  void DeserializeRecursive();

  void ExtractToDiskRecursive(std::filesystem::path File);
  void UnfoldToDiskRecursive(std::filesystem::path File);
  void PrintTableOfContentRecursive(std::function<void(char const *)> Callback, uint32_t Offset, uint32_t Indent, uint32_t Increment);
  void PrintOfTypeRecursive(std::function<void(char const *)> Callback, std::string const &Type);
  void FindArchiveByTypeRecursive(std::string const &Type, NpArchive **Archive);
  void FindArchiveByNameRecursive(std::string const &Name, NpArchive **Archive);
  void FindArchiveByTypeAndNameRecursive(std::string const &Type, std::string const &Name, NpArchive **Archive);
  void FindArchivesByTypeRecursive(std::string const &Type, std::vector<NpArchive *> &Archives);
  void FindArchivesByNameRecursive(std::string const &Name, std::vector<NpArchive *> &Archives);

  bool CheckIfDirectory(NpBinaryProcessor *Processor);
  void CreateDirectory(NpBinaryProcessor *Processor);
  void WriteDirectoryHeader(NpBinaryProcessor *Processor);
  void WriteFileContent(NpBinaryProcessor *Processor, NpArchive *Archive);

  uint64_t UpdateSizesRecursive();
  void UpdateByteArraysRecursive();

  std::string CreateUniqueFileName();

private:
  uint8_t *m_Bytes = nullptr;

  uint64_t m_Size = 0;
  uint64_t m_SizePrev = 0;

  NpArchive *m_Parent = nullptr;
  std::vector<NpArchive *> m_Children = {};

  char m_Type[4 + 1] = {};
  char m_Name[20 + 1] = {};
  uint32_t m_ParentOffset = 0;
  uint16_t m_ParentIndex = 0;

  bool m_IsDirectory = false;

  std::set<std::string> const m_KnownDirectoryTypes = {
      "", // Empty directory types exist
      "AKT",
      "BIN",
      "CMP",
      "DAT",
      "DDP",
      "EFF",
      "IDD",
      "PAC",
      "SCP",
      "TBL",
  };
  std::set<std::string> const m_KnownFileTypes = {
      "", // Empty file types exist
      "A00",
      "A01",
      "ACT",
      "AK",
      "ANS",
      "ANC",
      "B00",
      "B01",
      "BMH",
      "C00",
      "CAM",
      "CCH",
      "D00",
      "DDS",
      "EAR",
      "ECT",
      "EFP",
      "EMD",
      "EST",
      "FI2",
      "FIS",
      "ICO",
      "IDP",
      "ISL",
      "ITS",
      "JMP",
      "LI3",
      "MD",
      "MEH",
      "MOT",
      "MRT",
      "MSA",
      "MSD",
      "MSS",
      "PSN",
      "RHT",
      "RNI",
      "ROF",
      "S00",
      "S01",
      "S02",
      "S03",
      "S04",
      "S05",
      "SCA",
      "SCI",
      "SCL",
      "SCM",
      "SCR",
      "SEH",
      "SEQ",
      "SES",
      "SSD",
      "SSL",
      "TAT",
      "TRE",
      "TS",
      "TSC",
      "V00",
      "V01",
      "V02",
      "V03",
      "VET",
  };
};

#endif
