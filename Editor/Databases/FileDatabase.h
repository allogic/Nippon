#pragma once

#include <string>
#include <cstring>
#include <filesystem>
#include <vector>
#include <set>
#include <map>
#include <sstream>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class FileInfo
	{
	public:

		friend class FileDatabase;

	public:

		inline const auto& GetCrc32() const { return mCrc32; }
		inline const auto& GetRelativeFile() const { return mRelativeFile; }

	private:

		U32 mCrc32 = 0;

		I8 mRelativeFile[32] = {};
	};

	class FileContainer
	{
	public:

		friend class FileDatabase;

	public:

		enum FileType : U32
		{
			eFileTypeNone,
			eFileTypeLevel,
			eFileTypeEntity,
		};

	public:

		inline const auto& GetIdentifier() const { return mIdentifier; }
		inline const auto& GetType() const { return mType; }
		inline const auto& GetDirectoryId() const { return mDirectoryId; }
		inline const auto& GetFileId() const { return mFileId; }
		inline const auto& GetDirectoryName() const { return mDirectoryName; }
		inline const auto& GetFileName() const { return mFileName; }
		inline const auto& GetWindowName() const { return mWindowName; }
		inline const auto& GetDatFile() const { return mDatFile; }
		inline const auto& GetDatFFile() const { return mDatFFile; }
		inline const auto& GetDatGFile() const { return mDatGFile; }
		inline const auto& GetDatJFile() const { return mDatJFile; }
		inline const auto& GetDat20File() const { return mDat20File; }
		inline const auto& GetDat20FFile() const { return mDat20FFile; }
		inline const auto& GetDat20GFile() const { return mDat20GFile; }
		inline const auto& GetDat20JFile() const { return mDat20JFile; }
		inline const auto& GetBinFile() const { return mBinFile; }
		inline const auto& GetBinFFile() const { return mBinFFile; }
		inline const auto& GetBinGFile() const { return mBinGFile; }
		inline const auto& GetBinJFile() const { return mBinJFile; }
		inline const auto& GetBin20File() const { return mBin20File; }
		inline const auto& GetBin20FFile() const { return mBin20FFile; }
		inline const auto& GetBin20GFile() const { return mBin20GFile; }
		inline const auto& GetBin20JFile() const { return mBin20JFile; }

	private:

		U32 mIdentifier = 0;

		FileType mType = eFileTypeNone;

		I8 mDirectoryId[3] = {};
		I8 mFileId[3] = {};

		I8 mDirectoryName[256] = {};
		I8 mFileName[256] = {};
		I8 mWindowName[256] = {};

		FileInfo mDatFile = {};
		FileInfo mDatFFile = {};
		FileInfo mDatGFile = {};
		FileInfo mDatJFile = {};

		FileInfo mDat20File = {};
		FileInfo mDat20FFile = {};
		FileInfo mDat20GFile = {};
		FileInfo mDat20JFile = {};

		FileInfo mBinFile = {};
		FileInfo mBinFFile = {};
		FileInfo mBinGFile = {};
		FileInfo mBinJFile = {};

		FileInfo mBin20File = {};
		FileInfo mBin20FFile = {};
		FileInfo mBin20GFile = {};
		FileInfo mBin20JFile = {};
	};

	class FileDatabase
	{
	public:

		static void Create();
		static void Generate();
		static void Destroy();

	public:

		static U32 GetTotalCount();
		static U32 GetTotalLevelCount();
		static U32 GetTotalEntityCount();

		static const std::set<U16>& GetAllDirectories();
		static const std::set<U16>& GetLevelDirectories();
		static const std::set<U16>& GetEntityDirectories();

		static const I8* GetDirectoryIdByDirectory(U16 Directory);
		static const I8* GetDirectoryNameByDirectory(U16 Directory);

		static const std::vector<const FileContainer*>& GetFileContainersByDirectory(U16 Directory);
		static const std::vector<const FileContainer*>& GetLevelFileContainersByDirectory(U16 Directory);
		static const std::vector<const FileContainer*>& GetEntityFileContainersByDirectory(U16 Directory);

	private:

		static std::map<U32, std::string> LoadLabels();
		static void BuildFileContainersForDirectories(const std::set<std::string>& Directories, std::map<U32, std::string>& Labels, FileContainer::FileType Type, std::map<U32, FileContainer>& FileContainers);
		static void LoadFileContainers();
	};
}