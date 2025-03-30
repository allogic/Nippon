#if !defined(NP_CORE_ARCHIVE_HPP)
#  define NP_CORE_ARCHIVE_HPP

class NpArchive : public std::vector<NpArchive *> {
public:
  NpArchive();
  NpArchive(NpArchive *Parent);
  virtual ~NpArchive();

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
