#pragma once

#include <Standard.h>
#include <Types.h>

#define ARCHIVE_THUMBNAIL_SIZE 128

namespace Nippon
{
	enum ArchiveType : U32
	{
		eArchiveTypeLevel,
		eArchiveTypeEntity,
	};

	struct Configuration
	{
		fs::path DataDir;
	};

	struct FolderInfo
	{
		std::string FolderId;
		std::string FolderName;

		inline std::string const& GetFolderName() const { return (FolderName == "Unknown") ? FolderId : FolderName; }
	};

	struct ArchiveInfo
	{
		U32 UniqueId;
		ArchiveType ArchiveType;
		std::string FolderId;
		std::string ArchiveId;
		std::string FolderName;
		std::string ArchiveName;
		std::string WindowName;
		std::string FilePath;

		inline std::string const& GetFolderName() const { return (FolderName == "Unknown") ? FolderId : FolderName; }
		inline std::string const& GetArchiveName() const { return (ArchiveName == "Unknown") ? ArchiveId : ArchiveName; }
	};

	class Database
	{
	public:

		static void Create();
		static void Destroy();

	public:

		static bool IsFound();
		static bool IsReady();
		static bool IsBusy();

	public:

		static void SetCancelJob();
		static void SetArchiveCacheInvalid();
		static void SetThumbnailCacheInvalid();

	public:

		static void RebuildFull();
		static void RebuildArchives();
		static void RebuildThumbnails();

	public:

		static Configuration const& GetConfiguration();
		static void SetConfiguration(Configuration const& Configuration);

	public:

		static std::vector<FolderInfo> const& GetFolderInfos(bool QueryCache = true);
		static std::vector<FolderInfo> const& GetLevelFolderInfos(bool QueryCache = true);
		static std::vector<FolderInfo> const& GetEntityFolderInfos(bool QueryCache = true);

		static std::vector<ArchiveInfo> const& GetAllArchiveInfos(bool QueryCache = true);
		static std::vector<ArchiveInfo> const& GetLevelArchiveInfos(bool QueryCache = true);
		static std::vector<ArchiveInfo> const& GetEntityArchiveInfos(bool QueryCache = true);

		static std::vector<ArchiveInfo> const& GetArchiveInfosByFolderId(std::string const& FolderId, bool QueryCache = true);

		static std::vector<U8> const& GetArchiveDataByUniqueId(U32 UniqueId, bool QueryCache = true);
		static std::vector<U8> const& GetThumbnailDataByArchiveId(U32 ArchiveId, bool QueryCache = true);

		static U32 GetThumbnailTextureByArchiveId(U32 ArchiveId, bool QueryCache = true);

	public:

		static U32 GetUniqueIdByFolderIdAndArchiveId(std::string const& FolderId, std::string const& ArchiveId);

	private:

		static void RebuildFullDatabaseProcedure();
		static void RebuildArchiveDatabaseProcedure();
		static void RebuildThumbnailDatabaseProcedure();

	private:

		static void CreateConfigurationTable();
		static void CreateArchiveTable();
		static void CreateThumbnailTable();

		static void DropConfigurationTable();
		static void DropArchiveTable();
		static void DropThumbnailTable();

	private:

		static std::string GetFolderNameForFolderId(std::string const& FolderId);
		static std::string GetArchiveNameForFolderIdAndArchiveId(std::string const& FolderId, std::string const& ArchiveId);

	public:

		static void GenerateImGuiIni();

	private:

		static bool InsertLevelForPath(fs::path const& DirPath);
		static bool InsertEntityForPath(fs::path const& DirPath);

		static void InsertThumbnails(U32 Width, U32 Height);

	private:

		static bool CheckTableExistence(std::string const& TableName);
	};
}