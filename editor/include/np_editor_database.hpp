#if !defined(NP_EDITOR_DATABASE_HPP)
#  define NP_EDITOR_DATABASE_HPP

enum NpArchiveType {
  NP_ARCHIVE_TYPE_LEVEL,
  NP_ARCHIVE_TYPE_ENTITY,
};

class NpConfiguration {
};

struct NpFolderInfo {
  std::string FolderId;
  std::string FolderName;

  inline std::string const &GetFolderName() const { return (FolderName == "Unknown") ? FolderId : FolderName; }
};

struct NpArchiveInfo {
  uint32_t UniqueId;
  NpArchiveType ArchiveType;
  std::string FolderId;
  std::string ArchiveId;
  std::string FolderName;
  std::string ArchiveName;
  std::string WindowName;
  std::string FilePath;

  inline std::string const &GetFolderName() const { return (FolderName == "Unknown") ? FolderId : FolderName; }
  inline std::string const &GetArchiveName() const { return (ArchiveName == "Unknown") ? ArchiveId : ArchiveName; }
};

class NpDatabase {
public:
  NpDatabase();
  virtual ~NpDatabase();

  // bool IsFound();
  bool IsReady();
  bool IsBusy();

  void Create();
  void Destroy();

  // void SetCancelJob();
  void SetArchiveCacheInvalid();
  void SetThumbnailCacheInvalid();

  void RebuildFull();
  void RebuildArchives();
  void RebuildThumbnails();

  NpConfiguration const &GetConfiguration();
  void SetConfiguration(NpConfiguration const &Configuration);

  std::vector<NpFolderInfo> const &GetFolderInfos(bool QueryCache = true);
  std::vector<NpFolderInfo> const &GetLevelFolderInfos(bool QueryCache = true);
  std::vector<NpFolderInfo> const &GetEntityFolderInfos(bool QueryCache = true);

  std::vector<NpArchiveInfo> const &GetAllArchiveInfos(bool QueryCache = true);
  std::vector<NpArchiveInfo> const &GetLevelArchiveInfos(bool QueryCache = true);
  std::vector<NpArchiveInfo> const &GetEntityArchiveInfos(bool QueryCache = true);

  std::vector<NpArchiveInfo> const &GetArchiveInfosByFolderId(std::string const &FolderId, bool QueryCache = true);

  std::vector<uint8_t> const &GetArchiveBufferByUniqueId(uint32_t UniqueId, bool QueryCache = true);
  std::vector<uint8_t> const &GetThumbnailBufferByArchiveId(uint32_t ArchiveId, bool QueryCache = true);

  uint32_t GetThumbnailTextureByArchiveId(uint32_t ArchiveId, bool QueryCache = true);

  uint32_t GetUniqueIdByFolderIdAndArchiveId(std::string const &FolderId, std::string const &ArchiveId);

private:
  void RebuildFullDatabaseProcedure();
  void RebuildArchiveDatabaseProcedure();
  void RebuildThumbnailDatabaseProcedure();

  void CreateConfigurationTable();
  void CreateArchiveTable();
  void CreateThumbnailTable();

  void DropConfigurationTable();
  void DropArchiveTable();
  void DropThumbnailTable();

  std::string GetFolderNameForFolderId(std::string const &FolderId);
  std::string GetArchiveNameForFolderIdAndArchiveId(std::string const &FolderId, std::string const &ArchiveId);

  void GenerateImGuiIni();
  void GenerateModelInfoHeader();

  bool InsertLevelForPath(std::filesystem::path const &DirPath);
  bool InsertEntityForPath(std::filesystem::path const &DirPath);

  void InsertThumbnails(uint32_t Width, uint32_t Height);

  bool CheckTableExistence(std::string const &TableName);
};

extern NpDatabase g_Database;

#endif
