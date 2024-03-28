#include <Editor.h>

#include <Common/BlowFish.h>
#include <Common/Crc32.h>

#include <Database/Database.h>

#include <Ecs/Entity.h>
#include <Ecs/Registry.h>

#include <Ecs/Components/Camera.h>
#include <Ecs/Components/Transform.h>

#include <Glad/glad.h>

#include <Glfw/glfw3.h>

#include <Interface/Log.h>

#include <OpenGl/FrameBuffer.h>
#include <OpenGl/Texture2D.h>

#include <Scene/Scene.h>
#include <Scene/SceneManager.h>
#include <Scene/EntityScene.h>
#include <Scene/LevelScene.h>

#include <Sqlite/sqlite3.h>

#include <Utility/FileUtility.h>
#include <Utility/StringUtility.h>
#include <Utility/TextureUtility.h>

#ifdef _DEBUG
	#define SQL_CHECK(EXPRESSION, STATUS) \
		{ \
			U32 result = (EXPRESSION); \
			if (result != STATUS) \
			{ \
				Log::Add("[SQL] %s\n", sqlite3_errmsg(sDatabase)); \
			} \
		}
#else
	#define SQL_CHECK(EXPRESSION, ...) ((EXPRESSION))
#endif

namespace Nippon
{
	static sqlite3* sDatabase = nullptr;

	static std::atomic<bool> sDatabaseIsFound = false;
	static std::atomic<bool> sDatabaseIsReady = false;
	static std::atomic<bool> sDatabaseIsBusy = false;
	static std::atomic<bool> sDatabaseCancelJob = false;

	static bool sInitializedFolderInfos = false;
	static bool sInitializedLevelFolderInfos = false;
	static bool sInitializedEntityFolderInfos = false;

	static std::vector<FolderInfo> sCachedFolderInfos = {};
	static std::vector<FolderInfo> sCachedLevelFolderInfos = {};
	static std::vector<FolderInfo> sCachedEntityFolderInfos = {};

	static bool sInitializedArchiveInfos = false;
	static bool sInitializedLevelArchiveInfos = false;
	static bool sInitializedEntityArchiveInfos = false;

	static std::vector<ArchiveInfo> sCachedArchiveInfos = {};
	static std::vector<ArchiveInfo> sCachedLevelArchiveInfos = {};
	static std::vector<ArchiveInfo> sCachedEntityArchiveInfos = {};

	static std::map<std::string, bool> sInitializedArchiveInfosByFolderId = {};
	static std::map<std::string, std::vector<ArchiveInfo>> sCachedArchiveInfosByFolderId = {};

	static std::map<U32, bool> sInitializedArchiveDataByUniqueId = {};
	static std::map<U32, std::vector<U8>> sCachedArchiveDataByUniqueId = {};

	static std::map<U32, bool> sInitializedThumbnailDataByUniqueId = {};
	static std::map<U32, std::vector<U8>> sCachedThumbnailDataByUniqueId = {};

	static std::map<U32, bool> sInitializedThumbnailTextureByArchiveId = {};
	static std::map<U32, U32> sCachedThumbnailTextureByArchiveId = {};

	static I32 TableExistsCallback(void* Data, I32 Argc, char** Argv, char** ColumnName)
	{
		I32* tableExists = (I32*)Data;

		*tableExists = 1;

		return 0;
	}

	void Database::Create()
	{
		sDatabaseIsFound = fs::exists("Nippon.db");

		SQL_CHECK(sqlite3_open("Nippon.db", &sDatabase), SQLITE_OK);		

		CreateConfigurationTable();

		if (sDatabaseIsFound)
		{
			Log::Add("Found existing database\n");

			sDatabaseIsReady = CheckTableExistence("archives");

			if (sDatabaseIsReady)
			{
				Log::Add("Database is ready\n");
			}
			else
			{
				Log::Add("Database is not ready\n");
			}
		}
		else
		{
			Log::Add("Database is missing\n");
		}
	}

	void Database::Destroy()
	{
		for (auto const& [archiveId, textureId] : sCachedThumbnailTextureByArchiveId)
		{
			Texture2D::Destroy(textureId);
		}

		SQL_CHECK(sqlite3_close(sDatabase), SQLITE_OK);
	}

	bool Database::IsFound()
	{
		return sDatabaseIsFound;
	}

	bool Database::IsReady()
	{
		return sDatabaseIsReady;
	}

	bool Database::IsBusy()
	{
		return sDatabaseIsBusy;
	}

	void Database::SetCancelJob()
	{
		sDatabaseCancelJob = true;
	}

	void Database::SetArchiveCacheInvalid()
	{
		sInitializedFolderInfos = false;
		sInitializedLevelFolderInfos = false;
		sInitializedEntityFolderInfos = false;

		sInitializedArchiveInfos = false;
		sInitializedLevelArchiveInfos = false;
		sInitializedEntityArchiveInfos = false;

		sInitializedArchiveInfosByFolderId.clear();
		sInitializedArchiveDataByUniqueId.clear();
	}

	void Database::SetThumbnailCacheInvalid()
	{
		sInitializedThumbnailDataByUniqueId.clear();
		sInitializedThumbnailTextureByArchiveId.clear();
	}

	void Database::RebuildFull()
	{
		std::thread{ RebuildFullDatabaseProcedure }.detach();
	}

	void Database::RebuildArchives()
	{
		std::thread{ RebuildArchiveDatabaseProcedure }.detach();
	}

	void Database::RebuildThumbnails()
	{
		std::thread{ RebuildThumbnailDatabaseProcedure }.detach();
	}

	Configuration const& Database::GetConfiguration()
	{
		static Configuration configuration = {};

		std::stringstream sql = {};

		sql << "SELECT DISTINCT dataDir FROM configuration;";

		sqlite3_stmt* stmt = nullptr;

		SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			char const* dataDir = (char const*)sqlite3_column_text(stmt, 0);

			configuration.DataDir = dataDir;
		}

		SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

		return configuration;
	}

	void Database::SetConfiguration(Configuration const& Configuration)
	{
		std::stringstream sql = {};

		sql << R"(
			REPLACE INTO configuration (
				dataDir
			) VALUES (?);
		)";

		sqlite3_stmt* stmt = nullptr;

		SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

		std::string dataDir = Configuration.DataDir.string();

		SQL_CHECK(sqlite3_bind_text(stmt, 1, dataDir.data(), -1, SQLITE_STATIC), SQLITE_OK);

		SQL_CHECK(sqlite3_step(stmt), SQLITE_DONE);

		SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

		Log::Add("Updating configuration\n");
	}

	std::vector<FolderInfo> const& Database::GetFolderInfos(bool QueryCache)
	{
		if (QueryCache && sInitializedFolderInfos)
		{
			return sCachedFolderInfos;
		}
		else
		{
			sInitializedFolderInfos = true;

			std::stringstream sql = {};

			sql << "SELECT DISTINCT folderId, folderName FROM archives;";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedFolderInfos.clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				char const* folderId = (char const*)sqlite3_column_text(stmt, 0);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 1);

				FolderInfo folderInfo = { folderId, folderName };

				sCachedFolderInfos.emplace_back(folderInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedFolderInfos;
		}
	}

	std::vector<FolderInfo> const& Database::GetLevelFolderInfos(bool QueryCache)
	{
		if (QueryCache && sInitializedLevelFolderInfos)
		{
			return sCachedLevelFolderInfos;
		}
		else
		{
			sInitializedLevelFolderInfos = true;

			std::stringstream sql = {};

			sql << "SELECT DISTINCT folderId, folderName FROM archives WHERE archiveType=0;";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedLevelFolderInfos.clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				char const* folderId = (char const*)sqlite3_column_text(stmt, 0);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 1);

				FolderInfo folderInfo = { folderId, folderName };

				sCachedLevelFolderInfos.emplace_back(folderInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedLevelFolderInfos;
		}
	}

	std::vector<FolderInfo> const& Database::GetEntityFolderInfos(bool QueryCache)
	{
		if (QueryCache && sInitializedEntityFolderInfos)
		{
			return sCachedEntityFolderInfos;
		}
		else
		{
			sInitializedEntityFolderInfos = true;

			std::stringstream sql = {};

			sql << "SELECT DISTINCT folderId, folderName FROM archives WHERE archiveType=1;";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedEntityFolderInfos.clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				char const* folderId = (char const*)sqlite3_column_text(stmt, 0);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 1);

				FolderInfo folderInfo = { folderId, folderName };

				sCachedEntityFolderInfos.emplace_back(folderInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedEntityFolderInfos;
		}
	}

	std::vector<ArchiveInfo> const& Database::GetAllArchiveInfos(bool QueryCache)
	{
		if (QueryCache && sInitializedArchiveInfos)
		{
			return sCachedArchiveInfos;
		}
		else
		{
			sInitializedArchiveInfos = true;

			std::stringstream sql = {};

			sql << "SELECT uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath FROM archives;";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedArchiveInfos.clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				U32 uniqueId = sqlite3_column_int(stmt, 0);
				ArchiveType archiveType = (ArchiveType)sqlite3_column_int(stmt, 1);
				char const* folderId = (char const*)sqlite3_column_text(stmt, 2);
				char const* archiveId = (char const*)sqlite3_column_text(stmt, 3);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 4);
				char const* archiveName = (char const*)sqlite3_column_text(stmt, 5);
				char const* windowName = (char const*)sqlite3_column_text(stmt, 6);
				char const* filePath = (char const*)sqlite3_column_text(stmt, 7);

				ArchiveInfo archiveInfo = { uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath };

				sCachedArchiveInfos.emplace_back(archiveInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedArchiveInfos;
		}
	}

	std::vector<ArchiveInfo> const& Database::GetLevelArchiveInfos(bool QueryCache)
	{
		if (QueryCache && sInitializedLevelArchiveInfos)
		{
			return sCachedLevelArchiveInfos;
		}
		else
		{
			sInitializedLevelArchiveInfos = true;

			std::stringstream sql = {};

			sql << "SELECT uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath FROM archives WHERE archiveType=0;";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedLevelArchiveInfos.clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				U32 uniqueId = sqlite3_column_int(stmt, 0);
				ArchiveType archiveType = (ArchiveType)sqlite3_column_int(stmt, 1);
				char const* folderId = (char const*)sqlite3_column_text(stmt, 2);
				char const* archiveId = (char const*)sqlite3_column_text(stmt, 3);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 4);
				char const* archiveName = (char const*)sqlite3_column_text(stmt, 5);
				char const* windowName = (char const*)sqlite3_column_text(stmt, 6);
				char const* filePath = (char const*)sqlite3_column_text(stmt, 7);

				ArchiveInfo archiveInfo = { uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath };

				sCachedLevelArchiveInfos.emplace_back(archiveInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedLevelArchiveInfos;
		}
	}

	std::vector<ArchiveInfo> const& Database::GetEntityArchiveInfos(bool QueryCache)
	{
		if (QueryCache && sInitializedEntityArchiveInfos)
		{
			return sCachedEntityArchiveInfos;
		}
		else
		{
			sInitializedEntityArchiveInfos = true;

			std::stringstream sql = {};

			sql << "SELECT uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath FROM archives WHERE archiveType=1;";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedEntityArchiveInfos.clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				U32 uniqueId = sqlite3_column_int(stmt, 0);
				ArchiveType archiveType = (ArchiveType)sqlite3_column_int(stmt, 1);
				char const* folderId = (char const*)sqlite3_column_text(stmt, 2);
				char const* archiveId = (char const*)sqlite3_column_text(stmt, 3);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 4);
				char const* archiveName = (char const*)sqlite3_column_text(stmt, 5);
				char const* windowName = (char const*)sqlite3_column_text(stmt, 6);
				char const* filePath = (char const*)sqlite3_column_text(stmt, 7);

				ArchiveInfo archiveInfo = { uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath };

				sCachedEntityArchiveInfos.emplace_back(archiveInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedEntityArchiveInfos;
		}
	}

	std::vector<ArchiveInfo> const& Database::GetArchiveInfosByFolderId(std::string const& FolderId, bool QueryCache)
	{
		if (QueryCache && sInitializedArchiveInfosByFolderId[FolderId])
		{
			return sCachedArchiveInfosByFolderId[FolderId];
		}
		else
		{
			sInitializedArchiveInfosByFolderId[FolderId] = true;

			std::stringstream sql = {};

			sql << "SELECT uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath FROM archives WHERE folderId='" << FolderId << "';";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			sCachedArchiveInfosByFolderId[FolderId].clear();

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				U32 uniqueId = sqlite3_column_int(stmt, 0);
				ArchiveType archiveType = (ArchiveType)sqlite3_column_int(stmt, 1);
				char const* folderId = (char const*)sqlite3_column_text(stmt, 2);
				char const* archiveId = (char const*)sqlite3_column_text(stmt, 3);
				char const* folderName = (char const*)sqlite3_column_text(stmt, 4);
				char const* archiveName = (char const*)sqlite3_column_text(stmt, 5);
				char const* windowName = (char const*)sqlite3_column_text(stmt, 6);
				char const* filePath = (char const*)sqlite3_column_text(stmt, 7);

				ArchiveInfo archiveInfo = { uniqueId, archiveType, folderId, archiveId, folderName, archiveName, windowName, filePath };

				sCachedArchiveInfosByFolderId[FolderId].emplace_back(archiveInfo);
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedArchiveInfosByFolderId[FolderId];
		}
	}

	std::vector<U8> const& Database::GetArchiveDataByUniqueId(U32 UniqueId, bool QueryCache)
	{
		if (QueryCache && sInitializedArchiveDataByUniqueId[UniqueId])
		{
			return sCachedArchiveDataByUniqueId[UniqueId];
		}
		else
		{
			sInitializedArchiveDataByUniqueId[UniqueId] = true;

			std::stringstream sql = {};

			sql << "SELECT fileData, fileSize FROM archives WHERE uniqueId=" << UniqueId << ";";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				U8 const* fileData = (U8 const*)sqlite3_column_blob(stmt, 0);
				U32 fileSize = sqlite3_column_int(stmt, 1);

				sCachedArchiveDataByUniqueId[UniqueId] = { fileData, fileData + fileSize };
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedArchiveDataByUniqueId[UniqueId];
		}
	}

	std::vector<U8> const& Database::GetThumbnailDataByArchiveId(U32 ArchiveId, bool QueryCache)
	{
		if (QueryCache && sInitializedThumbnailDataByUniqueId[ArchiveId])
		{
			return sCachedThumbnailDataByUniqueId[ArchiveId];
		}
		else
		{
			sInitializedThumbnailDataByUniqueId[ArchiveId] = true;

			std::stringstream sql = {};

			sql << "SELECT fileData, fileSize FROM thumbnails WHERE archiveId=" << ArchiveId << ";";

			sqlite3_stmt* stmt = nullptr;

			SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				U8 const* fileData = (U8 const*)sqlite3_column_blob(stmt, 0);
				U32 fileSize = sqlite3_column_int(stmt, 1);

				sCachedThumbnailDataByUniqueId[ArchiveId] = { fileData, fileData + fileSize };
			}

			SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

			return sCachedThumbnailDataByUniqueId[ArchiveId];
		}
	}

	U32 Database::GetThumbnailTextureByArchiveId(U32 ArchiveId, bool QueryCache)
	{
		if (QueryCache && sInitializedThumbnailTextureByArchiveId[ArchiveId])
		{
			return sCachedThumbnailTextureByArchiveId[ArchiveId];
		}
		else
		{
			sInitializedThumbnailTextureByArchiveId[ArchiveId] = true;

			if (sCachedThumbnailTextureByArchiveId[ArchiveId])
			{
				Texture2D::Destroy(sCachedThumbnailTextureByArchiveId[ArchiveId]);
			}

			std::vector<U8> const& textureData = GetThumbnailDataByArchiveId(ArchiveId, false);

			sCachedThumbnailTextureByArchiveId[ArchiveId] = TextureUtility::ReadPNG(textureData.data(), textureData.size());

			return sCachedThumbnailTextureByArchiveId[ArchiveId];
		}
	}

	U32 Database::GetUniqueIdByFolderIdAndArchiveId(std::string const& FolderId, std::string const& ArchiveId)
	{
		U32 uniqueId = 0;

		std::stringstream sql = {};

		sql << "SELECT uniqueId FROM archives WHERE folderId='" << FolderId << "' AND archiveId='" << ArchiveId << "';";

		sqlite3_stmt* stmt = nullptr;

		SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			uniqueId = sqlite3_column_int(stmt, 0);
		}

		SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

		return uniqueId;
	}

	void Database::RebuildFullDatabaseProcedure()
	{
		sDatabaseIsBusy = true;
		sDatabaseIsReady = false;

		Log::Add("Started building database\n");

		DropArchiveTable();
		DropThumbnailTable();

		CreateArchiveTable();
		CreateThumbnailTable();

		SetArchiveCacheInvalid();
		SetThumbnailCacheInvalid();

		if (sDatabaseCancelJob)
		{
			Log::Add("Canceled building database\n");

			sDatabaseCancelJob = false;
		}
		else
		{
			Log::Add("Finished building database\n");
		}

		sDatabaseIsReady = CheckTableExistence("archives");
		sDatabaseIsBusy = false;
	}

	void Database::RebuildArchiveDatabaseProcedure()
	{
		sDatabaseIsBusy = true;
		sDatabaseIsReady = false;

		Log::Add("Started building archives\n");

		DropArchiveTable();

		CreateArchiveTable();

		SetArchiveCacheInvalid();

		if (sDatabaseCancelJob)
		{
			Log::Add("Canceled building archives\n");

			sDatabaseCancelJob = false;
		}
		else
		{
			Log::Add("Finished building archives\n");
		}

		sDatabaseIsReady = CheckTableExistence("archives");
		sDatabaseIsBusy = false;
	}

	void Database::RebuildThumbnailDatabaseProcedure()
	{
		sDatabaseIsBusy = true;
		sDatabaseIsReady = false;

		Log::Add("Started building thumbnails\n");

		DropThumbnailTable();

		CreateThumbnailTable();

		SetThumbnailCacheInvalid();

		if (sDatabaseCancelJob)
		{
			Log::Add("Canceled building thumbnails\n");

			sDatabaseCancelJob = false;
		}
		else
		{
			Log::Add("Finished building thumbnails\n");
		}

		sDatabaseIsReady = CheckTableExistence("archives");
		sDatabaseIsBusy = false;
	}

	void Database::CreateConfigurationTable()
	{
		std::stringstream sql = {};

		sql << R"(
			CREATE TABLE IF NOT EXISTS configuration (
				uniqueId INTEGER PRIMARY KEY,
				dataDir TEXT NOT NULL
			);
		)";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), nullptr, nullptr, nullptr), SQLITE_OK);

		Log::Add("Creating configuration table\n");
	}

	void Database::CreateArchiveTable()
	{
		std::stringstream sql = {};

		sql << R"(
			CREATE TABLE IF NOT EXISTS archives (
				uniqueId INTEGER PRIMARY KEY,
				archiveType INTEGER UNSIGNED NOT NULL,
				folderId VARCHAR(2) NOT NULL,
				archiveId VARCHAR(2) NOT NULL,
				folderName TEXT NOT NULL,
				archiveName TEXT NOT NULL,
				windowName TEXT NOT NULL,
				crc32 INTEGER UNSIGNED NOT NULL,
				filePath TEXT NOT NULL,
				fileData MEDIUMBLOB NOT NULL,
				fileSize INTEGER UNSIGNED NOT NULL
			);
		)";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), nullptr, nullptr, nullptr), SQLITE_OK);

		Log::Add("  Creating archives table\n");

		auto const& configuration = GetConfiguration();

		if (!InsertLevelForPath(configuration.DataDir / "st0")) return;
		if (!InsertLevelForPath(configuration.DataDir / "st1")) return;
		if (!InsertLevelForPath(configuration.DataDir / "st2")) return;
		if (!InsertLevelForPath(configuration.DataDir / "st3")) return;
		if (!InsertLevelForPath(configuration.DataDir / "stc")) return;
		if (!InsertLevelForPath(configuration.DataDir / "std")) return;
		if (!InsertLevelForPath(configuration.DataDir / "ste")) return;
		if (!InsertLevelForPath(configuration.DataDir / "stf")) return;

		if (!InsertEntityForPath(configuration.DataDir / "pl")) return;
		if (!InsertEntityForPath(configuration.DataDir / "em")) return;
		if (!InsertEntityForPath(configuration.DataDir / "an")) return;
		if (!InsertEntityForPath(configuration.DataDir / "it")) return;
		if (!InsertEntityForPath(configuration.DataDir / "dr")) return;
		if (!InsertEntityForPath(configuration.DataDir / "hm")) return;
		if (!InsertEntityForPath(configuration.DataDir / "hl")) return;
		if (!InsertEntityForPath(configuration.DataDir / "es")) return;
		if (!InsertEntityForPath(configuration.DataDir / "et")) return;
		if (!InsertEntityForPath(configuration.DataDir / "gt")) return;
		if (!InsertEntityForPath(configuration.DataDir / "us")) return;
		if (!InsertEntityForPath(configuration.DataDir / "ut")) return;
		if (!InsertEntityForPath(configuration.DataDir / "vt")) return;
		if (!InsertEntityForPath(configuration.DataDir / "wp")) return;
	}

	void Database::CreateThumbnailTable()
	{
		std::stringstream sql = {};

		sql << R"(
			CREATE TABLE IF NOT EXISTS thumbnails (
				uniqueId INTEGER PRIMARY KEY,
				archiveId INTEGER NOT NULL,
				fileData MEDIUMBLOB NOT NULL,
				fileSize INTEGER UNSIGNED NOT NULL,
				CONSTRAINT fkArchive
				FOREIGN KEY (archiveId)
					REFERENCES archives (uniqueId)
					ON DELETE SET NULL
			);
		)";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), nullptr, nullptr, nullptr), SQLITE_OK);

		Log::Add("  Creating thumbnails table\n");

		InsertThumbnails(ARCHIVE_THUMBNAIL_SIZE, ARCHIVE_THUMBNAIL_SIZE);
	}

	void Database::DropConfigurationTable()
	{
		std::stringstream sql = {};

		sql << "DROP TABLE IF EXISTS configuration;";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), nullptr, nullptr, nullptr), SQLITE_OK);

		Log::Add("Dropping configuration table\n");
	}

	void Database::DropArchiveTable()
	{
		std::stringstream sql = {};

		sql << "DROP TABLE IF EXISTS archives;";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), nullptr, nullptr, nullptr), SQLITE_OK);

		Log::Add("  Dropping archives table\n");
	}

	void Database::DropThumbnailTable()
	{
		std::stringstream sql = {};

		sql << "DROP TABLE IF EXISTS thumbnails;";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), nullptr, nullptr, nullptr), SQLITE_OK);

		Log::Add("  Dropping thumbnails table\n");
	}

	std::string Database::GetFolderNameForFolderId(std::string const& FolderId)
	{
		static std::map<std::string, std::string> folderNameByFolderId =
		{
			{ "r0", "Development" },
			{ "r1", "Eastern Nippon" },
			{ "r2", "Western Nippon" },
			{ "r3", "Kamui" },
			{ "rc", "Title Screen" },
			{ "rd", "Beta" },
			{ "re", "Fishing" },
			{ "rf", "Overworld" },

			{ "pl", "Player" },
			{ "em", "Enemies" },
			{ "an", "Animals" },
			{ "it", "Items" },
			{ "dr", "Doors" },
			{ "hm", "Humans" },
			{ "hl", "Humans" },
			{ "es", "Unknown" },
			{ "et", "Unknown" },
			{ "gt", "Unknown" },
			{ "us", "Unknown" },
			{ "ut", "Unknown" },
			{ "vt", "Unknown" },
			{ "wp", "Weapons" },
		};

		auto const& findIt = folderNameByFolderId.find(FolderId);

		return (findIt == folderNameByFolderId.end()) ? "Unknown" : folderNameByFolderId.at(FolderId);
	}

	std::string Database::GetArchiveNameForFolderIdAndArchiveId(std::string const& FolderId, std::string const& ArchiveId)
	{
		static std::map<std::string, std::map<std::string, std::string>> archiveNameByFolderIdAndArchiveId =
		{
			{
				"r0",
				{
					{ "06", "Endless load screen" },
					{ "07", "2005 Prototype Intro Cutscene Waka Lake" },
					{ "10", "Item Test Map 0x00-0x7F" },
					{ "11", "Item Test Map 0x80-0xFF" },
					{ "20", "Brush Interaction Test Map 1 Large Animals" },
					{ "21", "Brush Interaction Test Map 2 Small Animals" },
					{ "30", "Brush Interaction Test Map 3 Wheel Enemies" },
					{ "31", "Brush Interaction Test Map 4 Bull Enemies" },
					{ "32", "Brush Interaction Test Map 5 Empty" },
					{ "33", "Brush Interaction Test Map 6 Empty" },
					{ "34", "Brush Interaction Test Map 7 Blue Cylinder" },
					{ "ff", "Collision and Movement Test Map" },
				},
			},
			{
				"r1",
				{
					{ "00",	"Kamiki Village Game Start" },
					{ "01",	"Cave of Nagi" },
					{ "02",	"Kamiki Village" },
					{ "03",	"Hana Valley" },
					{ "04",	"Tsuta Ruins" },
					{ "05",	"City Checkpoint" },
					{ "06",	"Spider Queen Arena" },
					{ "07",	"Gale Shrine" },
					{ "08",	"Kusa Village" },
					{ "09",	"Sasa Sanctuary" },
					{ "0a",	"Madame Fawn's House" },
					{ "0b",	"Digging Minigame" },
					{ "0c",	"Onigiri Dojo Lesson Room" },
					{ "0d",	"Gale Shrine Crimson Helm Arena" },
					{ "0e",	"Calcified Cavern" },
					{ "10",	"Moon Cave Interior" },
					{ "11",	"Moon Cave Staircase and Orochi Arena" },
					{ "12",	"Kamiki Village After fighting Tei" },
					{ "13",	"Ryoshima Coast Bandit Spider Arena" },
					{ "14",	"Taka Pass Stray Bead #26 Cave" },
					{ "15",	"Kamui Cherry Bomb 3 Cave" },
					{ "16",	"N. Ryoshima Coast Power Slash 2 Cave" },
					{ "17",	"N. Ryoshima Coast Cherry Bomb 2 Cave" },
					{ "18",	"Kamui Ezofuji Power Slash 3 Cave" },
					{ "19",	"Kamui Blockhead Grande Cave" },
					{ "1a",	"N. Ryoshima Coast Bandit Spider Arena" },
					{ "1b",	"Kamui Bandit Spider Arena" },
					{ "1c",	"Dragon Palace Stray Bead #70 Cave" },
					{ "1d",	"N. Ryoshima Coast Stray Bead #63 Cave" },
					{ "20",	"New Game Intro Cutscene Map" },
					{ "21",	"Beta Kamiki" },
					{ "22",	"River of the Heavens" },
				},
			},
			{
				"r2",
				{
					{ "00", "Sei-an City Aristocratic Qtr." },
					{ "01", "Sei-an City Commoners Qtr." },
					{ "02", "Sei-an City Aristocratic Qtr." },
					{ "03", "Dragon Palace" },
					{ "04", "Inside the Dragon" },
					{ "05", "Sunken Ship" },
					{ "06", "Imperial Palace Ammy Size" },
					{ "07", "Imperial Palace Issun Size" },
					{ "08", "Oni Island Lower Interior" },
					{ "09", "Oni Island Ninetales Arena" },
					{ "0a", "Catcall Tower" },
					{ "0b", "Imperial Palace Blight Arena" },
					{ "0c", "Sei-an City Commoners Qtr." },
					{ "0d", "Oni Island Exterior" },
					{ "0e", "Oni Island Upper Interior" },
					{ "0f", "Oni Island Sidescroller" },
				},
			},
			{
				"r3",
				{
					{ "01", "Wep'keer" },
					{ "02", "Kamiki Village 100 years ago" },
					{ "03", "Wawku Shrine" },
					{ "04", "Wawku Shrine Nechku Arena" },
					{ "05", "Ponc'tan" },
					{ "06", "Moon Cave 100 years ago" },
					{ "07", "Ark of Yamato" },
					{ "08", "Ark of Yamato Spider Queen Arena" },
					{ "09", "Ark of Yamato Orochi Arena" },
					{ "0a", "Ark of Yamato Blight Arena" },
					{ "0b", "Ark of Yamato Ninetales Arena" },
					{ "0c", "Ark of Yamato Crimson Helm Arena" },
					{ "0d", "Ponc'tan Mrs. Seal's House" },
					{ "10", "Yoshpet" },
					{ "11", "Inner Yoshpet" },
					{ "12", "Ark of Yamato Yami Arena" },
					{ "13", "Wep'keer Square" },
					{ "14", "Wawku Shrine Lechku & Nechku Arena" },
				},
			},
			{
				"rc",
				{
					{ "00", "Return to Title Screen" },
					{ "02", "Presents from Issun" },
				},
			},
			{
				"rd",
				{
					{ "00", "Beta Shinshu" },
					{ "01", "Beta Hana" },
					{ "02", "Beta Tsuta" },
					{ "03", "Beta Agata" },
					{ "04", "Beta Ryoshima" },
					{ "05", "Beta Kamui" },
					{ "06", "Beta Taka" },
					{ "07", "Title Screen Demo Cutscene Village" },
				},
			},
			{
				"re",
				{
					{ "00", "Fishing with Benkei on Sei-an Bridge" },
					{ "01", "Fishing with Benkei next to Himiko's Palace" },
					{ "02", "Fishing with Kokari in Agata" },
					{ "03", "Fishing with Kokari in Kamui" },
					{ "04", "Fishing with Fisherman in NRyo" },
				}
			},
			{
				"rf",
				{
					{ "01", "Shinshu Field Cursed" },
					{ "02", "Shinshu Field Healed" },
					{ "03", "Agata Forest Cursed" },
					{ "04", "Agata Forest Healed" },
					{ "06", "Moon Cave Entrance" },
					{ "07", "Taka Pass Cursed" },
					{ "08", "Taka Pass Healed" },
					{ "09", "Ryoshima Coast Cursed" },
					{ "0a", "Ryoshima Coast Healed" },
					{ "0c", "N. Ryoshima Coast" },
					{ "11", "Kamui Cursed" },
					{ "12", "Kamui Healed" },
					{ "13", "Kamui Ezofuji" },
					{ "20", "Shinshu Field 100 years ago" },
					{ "21", "Moon Cave Entrance 100 years ago" },
				},
			},
		};

		auto const& folderFindIt = archiveNameByFolderIdAndArchiveId.find(FolderId);

		if (folderFindIt == archiveNameByFolderIdAndArchiveId.end())
		{
			return "Unknown";
		}
		else
		{
			std::map<std::string, std::string> const& archiveNameByArchiveId = folderFindIt->second;

			auto const& archiveFindIt = archiveNameByArchiveId.find(ArchiveId);

			if (archiveFindIt == archiveNameByArchiveId.end())
			{
				return "Unknown";
			}
			else
			{
				return archiveFindIt->second;
			}
		}
	}

	void Database::GenerateImGuiIni()
	{
		std::stringstream ss = {};

		ss << "[Window][DockSpaceViewport_11111111]\n";
		ss << "Pos=0,29\n";
		ss << "Size=1920,1051\n";
		ss << "Collapsed=0\n";
		ss << "\n";

		ss << "[Window][Debug##Default]\n";
		ss << "Pos=60,60\n";
		ss << "Size=400,400\n";
		ss << "Collapsed=0\n";
		ss << "\n";

		ss << "[Window][\xf3\xb0\xa4\x80 Inspector]\n";
		ss << "Pos=1320,29\n";
		ss << "Size=600,1051\n";
		ss << "Collapsed=0\n";
		ss << "DockId=0x00000008,0\n";
		ss << "\n";

		ss << "[Window][\xf3\xb0\x99\x85 Outline]\n";
		ss << "Pos=0,29\n";
		ss << "Size=317,1051\n";
		ss << "Collapsed=0\n";
		ss << "DockId=0x00000009,0\n";
		ss << "\n";

		ss << "[Window][\xf3\xb0\x80\xbc Archive Browser]\n";
		ss << "Pos=319,745\n";
		ss << "Size=999,335\n";
		ss << "Collapsed=0\n";
		ss << "DockId=0x0000000C,0\n";
		ss << "\n";

		ss << "[Window][\xf3\xb0\xa6\xaa Log]\n";
		ss << "Pos=1320,29\n";
		ss << "Size=600,1051\n";
		ss << "Collapsed=0\n";
		ss << "DockId=0x00000008,1\n";
		ss << "\n";

		auto const& archiveInfos = GetAllArchiveInfos(false);

		for (auto const& archiveInfo : archiveInfos)
		{
			ss << "[Window][\xf3\xb0\x95\xb4 " << archiveInfo.WindowName << "]\n";
			ss << "Pos=319,29\n";
			ss << "Size=999,714\n";
			ss << "Collapsed=0\n";
			ss << "DockId=0x0000000B,0\n";
			ss << "\n";
		}

		ss << "[Docking][Data]\n";
		ss << "DockSpace             ID=0x8B93E3BD Window=0xA787BDB4 Pos=651,215 Size=1920,1051 Split=X\n";
		ss << "  DockNode            ID=0x00000009 Parent=0x8B93E3BD SizeRef=317,1051 Selected=0x807052AF\n";
		ss << "  DockNode            ID=0x0000000A Parent=0x8B93E3BD SizeRef=1601,1051 Split=X\n";
		ss << "    DockNode          ID=0x00000007 Parent=0x0000000A SizeRef=999,1051 Split=X\n";
		ss << "      DockNode        ID=0x00000003 Parent=0x00000007 SizeRef=306,1051 Selected=0xC1DC7251\n";
		ss << "      DockNode        ID=0x00000004 Parent=0x00000007 SizeRef=1612,1051 Split=X\n";
		ss << "        DockNode      ID=0x00000001 Parent=0x00000004 SizeRef=999,1051 Split=Y\n";
		ss << "          DockNode    ID=0x00000005 Parent=0x00000001 SizeRef=985,711 Split=Y Selected=0xAE26A226\n";
		ss << "            DockNode  ID=0x0000000B Parent=0x00000005 SizeRef=1107,1003 CentralNode=1 Selected=0xE6E17602\n";
		ss << "            DockNode  ID=0x0000000C Parent=0x00000005 SizeRef=1107,335 Selected=0x73086A4A\n";
		ss << "          DockNode    ID=0x00000006 Parent=0x00000001 SizeRef=985,338 Selected=0x3C81409F\n";
		ss << "        DockNode      ID=0x00000002 Parent=0x00000004 SizeRef=611,1051 Selected=0x64F50EE5\n";
		ss << "    DockNode          ID=0x00000008 Parent=0x0000000A SizeRef=600,1051 Selected=0xC26E4521\n";

		FileUtility::WriteText("imgui.ini", ss.str());
	}

	bool Database::InsertLevelForPath(fs::path const& DirPath)
	{
		for (auto const& entry : fs::directory_iterator{ DirPath })
		{
			if (sDatabaseCancelJob)
			{
				return false;
			}

			fs::path file = entry.path();

			std::string stem = file.stem().string();
			std::string extension = file.extension().string();

			if ((extension == ".dat") && (stem.size() == 4))
			{
				std::string filePath = StringUtility::CutFront(file.string(), DirPath.string().size());

				std::string folderId = StringUtility::SelectExpr(stem, "XX");
				std::string archiveId = StringUtility::SelectExpr(stem, "??XX");

				std::string folderName = GetFolderNameForFolderId(folderId);
				std::string archiveName = GetArchiveNameForFolderIdAndArchiveId(folderId, archiveId);
				std::string windowName = "/" + folderId + "/" + archiveId + " - " + archiveName;

				std::vector<U8> fileData = FileUtility::ReadBinary(file.string());
				U32 fileSize = (U32)fileData.size();

				U32 crc32 = Crc32::FromRange(fileData.data(), fileData.size());

				BlowFish::Decrypt(fileData);

				std::stringstream sql = {};

				sql << R"(
					INSERT INTO archives (
						archiveType,
						folderId,
						archiveId,
						folderName,
						archiveName,
						windowName,
						crc32,
						filePath,
						fileData,
						fileSize
					) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
				)";

				sqlite3_stmt* stmt = nullptr;

				SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

				SQL_CHECK(sqlite3_bind_int(stmt, 1, 0), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 2, folderId.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 3, archiveId.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 4, folderName.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 5, archiveName.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 6, windowName.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_int(stmt, 7, crc32), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 8, filePath.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_blob(stmt, 9, fileData.data(), (U32)fileData.size(), SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_int(stmt, 10, fileSize), SQLITE_OK);

				SQL_CHECK(sqlite3_step(stmt), SQLITE_DONE);

				SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

				Log::Add("    Inserting level archive /%s/%s\n", folderId.data(), archiveId.data());
			}
		}

		return true;
	}

	bool Database::InsertEntityForPath(fs::path const& DirPath)
	{
		for (auto const& entry : fs::directory_iterator{ DirPath })
		{
			if (sDatabaseCancelJob)
			{
				return false;
			}

			fs::path file = entry.path();

			std::string stem = file.stem().string();
			std::string extension = file.extension().string();

			if ((extension == ".dat") && (stem.size() == 4))
			{
				std::string filePath = StringUtility::CutFront(file.string(), DirPath.string().size());

				std::string folderId = StringUtility::SelectExpr(stem, "XX");
				std::string archiveId = StringUtility::SelectExpr(stem, "??XX");

				std::string folderName = GetFolderNameForFolderId(folderId);
				std::string archiveName = GetArchiveNameForFolderIdAndArchiveId(folderId, archiveId);
				std::string windowName = "/" + folderId + "/" + archiveId + " - " + archiveName;

				std::vector<U8> fileData = FileUtility::ReadBinary(file.string());
				U32 fileSize = (U32)fileData.size();

				U32 crc32 = Crc32::FromRange(fileData.data(), fileData.size());

				BlowFish::Decrypt(fileData);

				std::stringstream sql = {};

				sql << R"(
					INSERT INTO archives (
						archiveType,
						folderId,
						archiveId,
						folderName,
						archiveName,
						windowName,
						crc32,
						filePath,
						fileData,
						fileSize
					) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
				)";

				sqlite3_stmt* stmt = nullptr;

				SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

				SQL_CHECK(sqlite3_bind_int(stmt, 1, 1), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 2, folderId.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 3, archiveId.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 4, folderName.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 5, archiveName.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 6, windowName.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_int(stmt, 7, crc32), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_text(stmt, 8, filePath.data(), -1, SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_blob(stmt, 9, fileData.data(), (U32)fileData.size(), SQLITE_STATIC), SQLITE_OK);
				SQL_CHECK(sqlite3_bind_int(stmt, 10, fileSize), SQLITE_OK);

				SQL_CHECK(sqlite3_step(stmt), SQLITE_DONE);

				SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

				Log::Add("    Inserting entity archive /%s/%s\n", folderId.data(), archiveId.data());
			}
		}

		return true;
	}

	void Database::InsertThumbnails(U32 Width, U32 Height)
	{
		if (glfwInit())
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_SAMPLES, 8);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		
			GLFWwindow* glfwContext = glfwCreateWindow(128, 128, "", nullptr, nullptr);
		
			if (glfwContext)
			{
				glfwMakeContextCurrent(glfwContext);
		
				gladLoadGL();

				std::vector<ArchiveInfo> archiveInfos = GetAllArchiveInfos(false);

				for (auto const& archiveInfo : archiveInfos)
				{
					if (sDatabaseCancelJob)
					{
						break;
					}

					if (Scene* scene = SceneManager::CreateScene(archiveInfo, true))
					{
						if (Registry* registry = scene->GetRegistry())
						{
							Entity* playerEntity = registry->GetPlayerEntity();

							Transform* playerTransform = playerEntity->GetTransform();

							playerTransform->SetPosition(R32V3{ 100.0F, 50.0F, 100.0F });
							playerTransform->SetRotation(R32V3{ -20.0F, 45.0F, 0.0F });
						}

						scene->Resize(Width, Height);
						scene->Load();
						scene->Invalidate();

						U32 frameBuffer = scene->GetFrameBuffer();
						U32 colorTexture = FrameBuffer::GetColorTexture(frameBuffer, 0);

						std::vector<U8> textureData = TextureUtility::WritePNG(colorTexture);

						std::stringstream sql = {};

						sql << R"(
							INSERT INTO thumbnails (
								archiveId,
								fileData,
								fileSize
							) VALUES (?, ?, ?);
						)";

						sqlite3_stmt* stmt = nullptr;

						SQL_CHECK(sqlite3_prepare_v2(sDatabase, sql.str().data(), -1, &stmt, nullptr), SQLITE_OK);

						SQL_CHECK(sqlite3_bind_int(stmt, 1, archiveInfo.UniqueId), SQLITE_OK);
						SQL_CHECK(sqlite3_bind_blob(stmt, 2, textureData.data(), (U32)textureData.size(), SQLITE_STATIC), SQLITE_OK);
						SQL_CHECK(sqlite3_bind_int(stmt, 3, (U32)textureData.size()), SQLITE_OK);

						SQL_CHECK(sqlite3_step(stmt), SQLITE_DONE);

						SQL_CHECK(sqlite3_finalize(stmt), SQLITE_OK);

						Log::Add("    Inserting thumbnail image /%s/%s\n", archiveInfo.FolderId.data(), archiveInfo.ArchiveId.data());

						SceneManager::DestroyScene(scene, true);
					}
				}
		
				glfwDestroyWindow(glfwContext);
			}
		}
	}

	bool Database::CheckTableExistence(std::string const& TableName)
	{
		I32 found = false;

		std::stringstream sql = {};

		sql << "SELECT name FROM sqlite_master WHERE type='table' AND name='" << TableName << "';";

		SQL_CHECK(sqlite3_exec(sDatabase, sql.str().data(), TableExistsCallback, &found, nullptr), SQLITE_OK);

		return found;
	}
}