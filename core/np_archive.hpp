#if !defined(NP_CORE_ARCHIVE_HPP)
#  define NP_CORE_ARCHIVE_HPP

class NpArchive : private std::vector<NpArchive *> {
public:
  NpArchive();
  NpArchive(NpArchive *Parent);
  virtual ~NpArchive();

public:
  inline auto const &IsDirectory() const { return m_IsDirectory; }
  inline auto const &GetBytes() const { return m_Bytes; }
  inline auto const &GetSize() const { return m_Size; }
  inline auto const &GetParent() const { return m_Parent; }
  inline auto const &GetType() const { return m_Type; }
  inline auto const &GetName() const { return m_Name; }
  inline auto const &GetParentOffset() const { return m_ParentOffset; }
  inline auto const &GetParentIndex() const { return m_ParentIndex; }
  /*
  public:
    void SetData(uint8_t const *Bytes, uint64_t Size);

  public:
    void Serialize(std::vector<uint8_t> &Bytes);
    void Deserialize(uint8_t const *Bytes, uint64_t Size);
    void Deserialize(std::vector<uint8_t> const &Bytes);

  public:
    void ExtractToDisk(fs::path const &File);
    void UnfoldToDisk(fs::path const &File);
    void PrintTableOfContent(std::function<void(char const *)> Callback, U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);
    void PrintOfType(std::function<void(char const *)> Callback, std::string const &Type);
    Archive *FindArchiveByType(std::string const &Type);
    Archive *FindArchiveByName(std::string const &Name);
    Archive *FindArchiveByTypeAndName(std::string const &Type, std::string const &Name);
    std::vector<Archive *> FindArchivesByType(std::string const &Type);
    std::vector<Archive *> FindArchivesByName(std::string const &Name);

  private:
    void SerializeRecursive();
    void DeserializeRecursive();

  private:
    void ExtractToDiskRecursive(fs::path File);
    void UnfoldToDiskRecursive(fs::path File);
    void PrintTableOfContentRecursive(std::function<void(char const *)> Callback, U32 Offset, U32 Indent, U32 Increment);
    void PrintOfTypeRecursive(std::function<void(char const *)> Callback, std::string const &Type);
    void FindArchiveByTypeRecursive(std::string const &Type, Archive **Result);
    void FindArchiveByNameRecursive(std::string const &Name, Archive **Result);
    void FindArchiveByTypeAndNameRecursive(std::string const &Type, std::string const &Name, Archive **Result);
    void FindArchivesByTypeRecursive(std::string const &Type, std::vector<Archive *> &Result);
    void FindArchivesByNameRecursive(std::string const &Name, std::vector<Archive *> &Result);

  private:
    bool CheckIfDirectory(BinaryMediator *Mediator);
    void CreateDirectory(BinaryMediator *Mediator);

  private:
    void WriteDirectoryHeader(BinaryMediator *Mediator);
    void WriteFileContent(BinaryMediator *Mediator, Archive *Archive);

  private:
    U64 UpdateSizesRecursive();
    void UpdateByteArraysRecursive();
  */

private:
  uint8_t *m_Bytes = nullptr;

  uint64_t m_Size = 0;
  uint64_t m_SizePrev = 0;

  NpArchive *m_Parent = nullptr;

  int8_t m_Type[4 + 1] = {};
  int8_t m_Name[20 + 1] = {};
  uint32_t m_ParentOffset = 0;
  uint16_t m_ParentIndex = 0;

  bool m_IsDirectory = false;
};

#endif
