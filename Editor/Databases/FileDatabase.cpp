#include <Common/Macros.h>
#include <Common/BinaryReader.h>
#include <Common/CRC32.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

#include <Editor/Editor.h>

#include <Editor/Databases/FileDatabase.h>

namespace ark
{
	static std::set<U16> sLevelDirectories = {};
	static std::set<U16> sEntityDirectories = {};

	static std::map<U16, const I8*> sLevelDirectoryIdsByDirectory = {};
	static std::map<U16, const I8*> sEntityDirectoryIdsByDirectory = {};

	static std::map<U16, const I8*> sLevelDirectoryNamesByDirectory = {};
	static std::map<U16, const I8*> sEntityDirectoryNamesByDirectory = {};

	static U32 sTotalLevelCount = 0;
	static U32 sTotalEntityCount = 0;

	static std::map<U16, std::vector<FileContainer>> sLevelsByDirectory = {};
	static std::map<U16, std::vector<FileContainer>> sEntitiesByDirectory = {};

	void FileDatabase::Create()
	{
		LoadFileContainers();
	}

	void FileDatabase::Generate()
	{
		std::map<U32, FileContainer> fileContainers = {};

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

		for (const auto& dir : levelDirectories)
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
				std::string windowName = "\\" + directoryId + "\\" + fileId + " " + labels[containerIdentifier];

				auto& fileContainer = fileContainers[containerIdentifier];

				fileContainer.mIdentifier = containerIdentifier;
				fileContainer.mType = FileContainer::eFileTypeLevel;

				std::memcpy(fileContainer.mDirectoryId, directoryId.c_str(), directoryId.size());
				std::memcpy(fileContainer.mFileId, fileId.c_str(), fileId.size());

				std::memcpy(fileContainer.mDirectoryName, labels[directoryIdentifier].c_str(), labels[directoryIdentifier].size());
				std::memcpy(fileContainer.mFileName, labels[containerIdentifier].c_str(), labels[containerIdentifier].size());
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

		for (const auto& dir : entityDirectories)
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
				std::string windowName = "\\" + directoryId + "\\" + fileId + " " + labels[containerIdentifier];

				auto& fileContainer = fileContainers[containerIdentifier];

				fileContainer.mIdentifier = containerIdentifier;
				fileContainer.mType = FileContainer::eFileTypeEntity;

				std::memcpy(fileContainer.mDirectoryId, directoryId.c_str(), directoryId.size());
				std::memcpy(fileContainer.mFileId, fileId.c_str(), fileId.size());

				std::memcpy(fileContainer.mDirectoryName, labels[directoryIdentifier].c_str(), labels[directoryIdentifier].size());
				std::memcpy(fileContainer.mFileName, labels[containerIdentifier].c_str(), labels[containerIdentifier].size());
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

		fs::path databaseFile = "FileDatabase.db";

		std::ofstream stream = std::ofstream{ databaseFile, std::ios::binary };

		if (stream.is_open())
		{
			U32 entryCount = (U32)fileContainers.size();

			stream.write((const I8*)&entryCount, sizeof(U32));

			for (const auto& [_, fileContainer] : fileContainers)
			{
				stream.write((const I8*)&fileContainer, sizeof(FileContainer));
			}

			stream.close();
		}
	}


	void FileDatabase::Destroy()
	{

	}

	U32 FileDatabase::GetTotalLevelCount()
	{
		return sTotalLevelCount;
	}

	U32 FileDatabase::GetTotalEntityCount()
	{
		return sTotalEntityCount;
	}

	const std::set<U16>& FileDatabase::GetLevelDirectories()
	{
		return sLevelDirectories;
	}

	const std::set<U16>& FileDatabase::GetEntityDirectories()
	{
		return sEntityDirectories;
	}

	const I8* FileDatabase::GetLevelDirectoryIdByDirectory(U16 Directory)
	{
		return sLevelDirectoryIdsByDirectory[Directory];
	}

	const I8* FileDatabase::GetEntityDirectoryIdByDirectory(U16 Directory)
	{
		return sEntityDirectoryIdsByDirectory[Directory];
	}

	const I8* FileDatabase::GetLevelDirectoryNameByDirectory(U16 Directory)
	{
		return sLevelDirectoryNamesByDirectory[Directory];
	}

	const I8* FileDatabase::GetEntityDirectoryNameByDirectory(U16 Directory)
	{
		return sEntityDirectoryNamesByDirectory[Directory];
	}

	const std::vector<FileContainer>& FileDatabase::GetLevelFileContainersByDirectory(U16 Directory)
	{
		return sLevelsByDirectory[Directory];
	}

	const std::vector<FileContainer>& FileDatabase::GetEntityFileContainersByDirectory(U16 Directory)
	{
		return sEntitiesByDirectory[Directory];
	}

	std::map<U32, std::string> FileDatabase::LoadLabels()
	{
		std::map<U32, std::string> labels = {};

		fs::path labelsFile = "Labels.txt";

		if (fs::exists(labelsFile))
		{
			auto fileStream = std::istringstream{ FsUtils::ReadText(labelsFile) };

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
			LOG("Labels file %s does not exist\n", labelsFile.string().c_str());
		}

		return labels;
	}

	void FileDatabase::LoadFileContainers()
	{
		fs::path databaseFile = "FileDatabase.db";

		if (fs::exists(databaseFile))
		{
			std::vector<U8> bytes = FsUtils::ReadBinary(databaseFile);

			BinaryReader reader = { bytes.data(), bytes.size() };

			U32 entryCount = reader.Read<U32>();

			for (U32 i = 0; i < entryCount; i++)
			{
				FileContainer fileContainerTmp = reader.Read<FileContainer>();

				U16 directory = (fileContainerTmp.mIdentifier >> 16) & 0xFFFF;

				switch (fileContainerTmp.mType)
				{
					case FileContainer::eFileTypeLevel:
					{
						sLevelDirectories.emplace(directory);

						FileContainer& fileContainer = sLevelsByDirectory[directory].emplace_back(fileContainerTmp);

						sLevelDirectoryIdsByDirectory[directory] = fileContainer.mDirectoryId;
						sLevelDirectoryNamesByDirectory[directory] = fileContainer.mDirectoryName;

						sTotalLevelCount += 1;

						break;
					}
					case FileContainer::eFileTypeEntity:
					{
						sEntityDirectories.emplace(directory);

						FileContainer& fileContainer = sEntitiesByDirectory[directory].emplace_back(fileContainerTmp);

						sEntityDirectoryIdsByDirectory[directory] = fileContainer.mDirectoryId;
						sEntityDirectoryNamesByDirectory[directory] = fileContainer.mDirectoryName;

						sTotalEntityCount += 1;

						break;
					}
				}
			}
		}
		else
		{
			LOG("File database %s does not exist\n", databaseFile.string().c_str());
		}
	}
}
