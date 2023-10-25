#include <Common/Macros.h>
#include <Common/BinaryMediator.h>
#include <Common/CRC32.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

#include <Editor/Editor.h>

#include <Editor/Databases/FileDatabase.h>

#define FILE_LABELS_NAME "Labels.txt"
#define FILE_DATABASE_NAME "FileDatabase.db"

namespace ark
{
	static U32 sTotalCount = 0;
	static U32 sTotalLevelCount = 0;
	static U32 sTotalEntityCount = 0;

	static std::set<U16> sAllDirectories = {};
	static std::set<U16> sLevelDirectories = {};
	static std::set<U16> sEntityDirectories = {};

	static std::map<U16, const I8*> sDirectoryIdByDirectory = {};
	static std::map<U16, const I8*> sDirectoryNameByDirectory = {};

	static std::map<U16, std::vector<const FileContainer*>> sFileContainersByDirectory = {};
	static std::map<U16, std::vector<const FileContainer*>> sLevelFileContainersByDirectory = {};
	static std::map<U16, std::vector<const FileContainer*>> sEntityFileContainersByDirectory = {};

	void FileDatabase::Create()
	{
		LoadFileContainers();
	}

	void FileDatabase::Generate()
	{
		std::set<std::string> levelDirectories =
		{
			"st0",
			"st1",
			"st2",
			"st3",
			"stc",
			"std",
			"ste",
			"stf",
		};

		std::set<std::string> entityDirectories =
		{
			"pl",
			"em",
			"an",
			"it",
			"dr",
			"hm",
			"hl",
			"es",
			"et",
			"gt",
			"us",
			"ut",
			"vt",
			"wp",
		};

		std::map<U32, std::string> labels = LoadLabels();

		std::map<U32, FileContainer> fileContainers = {};

		BuildFileContainersForDirectories(levelDirectories, labels, FileContainer::eFileTypeLevel, fileContainers);
		BuildFileContainersForDirectories(entityDirectories, labels, FileContainer::eFileTypeEntity, fileContainers);

		std::vector<U8> bytes = {};

		bytes.resize(4 * 1024 * 1024);

		BinaryMediator mediator = { bytes.data(), bytes.size() };

		mediator.Write<U32>((U32)fileContainers.size());

		for (const auto& [identifier, fileContainer] : fileContainers)
		{
			mediator.Write<FileContainer>(fileContainer);
		}

		FsUtils::WriteBinary(FILE_DATABASE_NAME, bytes);
	}

	void FileDatabase::Destroy()
	{
		for (const auto& [directory, fileContainers] : sFileContainersByDirectory)
		{
			for (const auto& fileContainer : fileContainers)
			{
				delete fileContainer;
			}
		}
	}

	U32 FileDatabase::GetTotalCount()
	{
		return sTotalCount;
	}

	U32 FileDatabase::GetTotalLevelCount()
	{
		return sTotalLevelCount;
	}

	U32 FileDatabase::GetTotalEntityCount()
	{
		return sTotalEntityCount;
	}

	const std::set<U16>& FileDatabase::GetAllDirectories()
	{
		return sAllDirectories;
	}

	const std::set<U16>& FileDatabase::GetLevelDirectories()
	{
		return sLevelDirectories;
	}

	const std::set<U16>& FileDatabase::GetEntityDirectories()
	{
		return sEntityDirectories;
	}

	const I8* FileDatabase::GetDirectoryIdByDirectory(U16 Directory)
	{
		return sDirectoryIdByDirectory[Directory];
	}

	const I8* FileDatabase::GetDirectoryNameByDirectory(U16 Directory)
	{
		return sDirectoryNameByDirectory[Directory];
	}

	const std::vector<const FileContainer*>& FileDatabase::GetFileContainersByDirectory(U16 Directory)
	{
		return sFileContainersByDirectory[Directory];
	}

	const std::vector<const FileContainer*>& FileDatabase::GetLevelFileContainersByDirectory(U16 Directory)
	{
		return sLevelFileContainersByDirectory[Directory];
	}

	const std::vector<const FileContainer*>& FileDatabase::GetEntityFileContainersByDirectory(U16 Directory)
	{
		return sEntityFileContainersByDirectory[Directory];
	}

	void FileDatabase::BuildFileContainersForDirectories(const std::set<std::string>& Directories, std::map<U32, std::string>& Labels, FileContainer::FileType Type, std::map<U32, FileContainer>& FileContainers)
	{
		for (const auto& dir : Directories)
		{
			for (const auto file : fs::directory_iterator{ gDataDir / dir })
			{
				std::vector<U8> bytes = FsUtils::ReadBinary(file);

				U32 crc32 = CRC32::FromRange(bytes.data(), bytes.size());

				std::string fileName = file.path().stem().string();
				std::string fileExt = file.path().extension().string();
				std::string folderName = file.path().parent_path().filename().string();

				std::string directoryId = StringUtils::SelectExpr(fileName, "XX??");
				std::string fileId = StringUtils::SelectExpr(fileName, "??XX");

				LOG("Generating for \\%s\\%s\n", directoryId.c_str(), fileId.c_str());

				U32 directoryIdentifier = (directoryId[0] << 24) | (directoryId[1] << 16) | (0xFF << 8);
				U32 containerIdentifier = (directoryId[0] << 24) | (directoryId[1] << 16) | StringUtils::HexStrToByte(fileId);

				std::string relativeFile = folderName + "\\" + fileName + fileExt;
				std::string windowName = "\\" + directoryId + "\\" + fileId + " " + Labels[containerIdentifier];

				auto& fileContainer = FileContainers[containerIdentifier];

				fileContainer.mIdentifier = containerIdentifier;
				fileContainer.mType = Type;

				std::memcpy(fileContainer.mDirectoryId, directoryId.c_str(), 2);
				std::memcpy(fileContainer.mFileId, fileId.c_str(), 2);

				std::memcpy(fileContainer.mDirectoryName, Labels[directoryIdentifier].c_str(), Labels[directoryIdentifier].size());
				std::memcpy(fileContainer.mFileName, Labels[containerIdentifier].c_str(), Labels[containerIdentifier].size());
				std::memcpy(fileContainer.mWindowName, windowName.c_str(), windowName.size());

				std::string chr1 = StringUtils::SelectExpr(fileName, "????XX");
				std::string p20 = StringUtils::SelectExpr(fileName, "????XXX");

				if (fileExt == ".dat")
				{
					if (chr1 == "_f")
					{
						fileContainer.mDatFFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mDatFFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
					else if (chr1 == "_g")
					{
						fileContainer.mDatGFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mDatGFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
					else if (chr1 == "_j")
					{
						fileContainer.mDatJFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mDatJFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
					else if (p20 == "_20")
					{
						std::string chr2 = StringUtils::SelectExpr(fileName, "???????XX");

						if (chr2 == "_f")
						{
							fileContainer.mDat20FFile.mCrc32 = crc32;
							std::memcpy(fileContainer.mDat20FFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
						else if (chr2 == "_g")
						{
							fileContainer.mDat20GFile.mCrc32 = crc32;
							std::memcpy(fileContainer.mDat20GFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
						else if (chr2 == "_j")
						{
							fileContainer.mDat20JFile.mCrc32 = crc32;
							std::memcpy(fileContainer.mDat20JFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
						else
						{
							fileContainer.mDat20File.mCrc32 = crc32;
							std::memcpy(fileContainer.mDat20File.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
					}
					else
					{
						fileContainer.mDatFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mDatFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
				}

				if (fileExt == ".bin")
				{
					if (chr1 == "_f")
					{
						fileContainer.mBinFFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mBinFFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
					else if (chr1 == "_g")
					{
						fileContainer.mBinGFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mBinGFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
					else if (chr1 == "_j")
					{
						fileContainer.mBinJFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mBinJFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
					else if (p20 == "_20")
					{
						std::string chr2 = StringUtils::SelectExpr(fileName, "???????XX");

						if (chr2 == "_f")
						{
							fileContainer.mBin20FFile.mCrc32 = crc32;
							std::memcpy(fileContainer.mBin20FFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
						else if (chr2 == "_g")
						{
							fileContainer.mBin20GFile.mCrc32 = crc32;
							std::memcpy(fileContainer.mBin20GFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
						else if (chr2 == "_j")
						{
							fileContainer.mBin20JFile.mCrc32 = crc32;
							std::memcpy(fileContainer.mBin20JFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
						else
						{
							fileContainer.mBin20File.mCrc32 = crc32;
							std::memcpy(fileContainer.mBin20File.mRelativeFile, relativeFile.c_str(), relativeFile.size());
						}
					}
					else
					{
						fileContainer.mBinFile.mCrc32 = crc32;
						std::memcpy(fileContainer.mBinFile.mRelativeFile, relativeFile.c_str(), relativeFile.size());
					}
				}
			}
		}
	}

	std::map<U32, std::string> FileDatabase::LoadLabels()
	{
		std::map<U32, std::string> labels = {};

		if (fs::exists(FILE_LABELS_NAME))
		{
			auto fileStream = std::istringstream{ FsUtils::ReadText(FILE_LABELS_NAME) };

			std::string line = "";

			while (std::getline(fileStream, line, '\n'))
			{
				std::vector<std::string> strings = {};

				auto lineStream = std::istringstream{ line };

				std::string string = "";

				while (std::getline(lineStream, string, ','))
				{
					strings.emplace_back(StringUtils::RemoveChars(string, '\"'));
				}

				switch (strings.size())
				{
					case 2:
					{
						const std::string& directoryId = strings[0];
						const std::string& label = strings[1];

						U32 identifier = (directoryId[0] << 24) | (directoryId[1] << 16) | (0xFF << 8);

						labels[identifier] = label;

						break;
					}
					case 3:
					{
						const std::string& directoryId = strings[0];
						const std::string& fileId = strings[1];
						const std::string& label = strings[2];

						U32 identifier = (directoryId[0] << 24) | (directoryId[1] << 16) | StringUtils::HexStrToByte(fileId);

						labels[identifier] = label;

						break;
					}
				}
			}
		}
		else
		{
			LOG("Labels file %s does not exist\n", FILE_LABELS_NAME);
		}

		return labels;
	}

	void FileDatabase::LoadFileContainers()
	{
		if (fs::exists(FILE_DATABASE_NAME))
		{
			std::vector<U8> bytes = FsUtils::ReadBinary(FILE_DATABASE_NAME);

			BinaryMediator mediator = { bytes.data(), bytes.size() };

			U32 entryCount = mediator.Read<U32>();

			for (U32 i = 0; i < entryCount; i++)
			{
				FileContainer* fileContainer = new FileContainer;

				FileContainer dummyFileContainer = mediator.Read<FileContainer>();

				std::memcpy(fileContainer, &dummyFileContainer, sizeof(FileContainer));

				U16 directory = (fileContainer->mIdentifier >> 16) & 0xFFFF;

				sFileContainersByDirectory[directory].emplace_back(fileContainer);

				sAllDirectories.emplace(directory);

				sDirectoryIdByDirectory[directory] = fileContainer->mDirectoryId;
				sDirectoryNameByDirectory[directory] = fileContainer->mDirectoryName;

				switch (fileContainer->mType)
				{
					case FileContainer::eFileTypeLevel:
					{
						sLevelDirectories.emplace(directory);

						sLevelFileContainersByDirectory[directory].emplace_back(fileContainer);

						sTotalLevelCount += 1;

						break;
					}
					case FileContainer::eFileTypeEntity:
					{
						sEntityDirectories.emplace(directory);

						sEntityFileContainersByDirectory[directory].emplace_back(fileContainer);

						sTotalEntityCount += 1;

						break;
					}
				}

				sTotalCount += 1;
			}
		}
		else
		{
			LOG("File database does not exist\n");
		}
	}
}
