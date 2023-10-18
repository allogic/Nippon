#include <string>
#include <filesystem>
#include <vector>
#include <set>

#include <Common/Macros.h>
#include <Common/Types.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

using namespace ark;

namespace fs = std::filesystem;

I32 main(I32 Argc, char** Argv)
{
	if ((Argc >= 2) && (std::strcmp(Argv[1], "Help") == 0))
	{
		LOG("\n");
		LOG("ArchiveAnalyzer.exe Decrypt <cipher-key> <input-file> <output-file>\n");
		LOG("ArchiveAnalyzer.exe Encrypt <cipher-key> <input-file> <output-file>\n");
		LOG("ArchiveAnalyzer.exe Unpack <input-file> <output-file>\n");
		LOG("ArchiveAnalyzer.exe ToC <input-file>\n");
		LOG("ArchiveAnalyzer.exe CollectTypes <input-file>\n");
		LOG("\n");
	}

	if ((Argc >= 5) && (std::strcmp(Argv[1], "Decrypt") == 0))
	{
		std::string cipherKey = Argv[2];

		fs::path inputFile = Argv[3];
		fs::path outputFile = Argv[4];

		if (cipherKey == "")
		{
			LOG("\n");
			LOG("Cipher key is missing\n");
			LOG("\n");
		}
		else
		{
			if (fs::exists(inputFile))
			{
				if (fs::exists(outputFile))
				{
					LOG("\n");
					LOG("Output file %s already exists\n", outputFile.string().c_str());
					LOG("\n");
				}
				else
				{
					BlowFish cipher = cipherKey;

					std::vector<U8> bytes = FsUtils::ReadBinary(inputFile.string());

					cipher.Decrypt(bytes);

					FsUtils::CreateIfNotExist(outputFile);
					FsUtils::WriteBinary(outputFile, bytes);
				}
			}
			else
			{
				LOG("\n");
				LOG("Input file %s does not exist\n", inputFile.string().c_str());
				LOG("\n");
			}
		}
	}

	if ((Argc >= 5) && (std::strcmp(Argv[1], "Encrypt") == 0))
	{
		std::string cipherKey = Argv[2];

		fs::path inputFile = Argv[3];
		fs::path outputFile = Argv[4];

		if (cipherKey == "")
		{
			LOG("\n");
			LOG("Cipher key is missing\n");
			LOG("\n");
		}
		else
		{
			if (fs::exists(inputFile))
			{
				if (fs::exists(outputFile))
				{
					LOG("\n");
					LOG("Output file %s already exists\n", outputFile.string().c_str());
					LOG("\n");
				}
				else
				{
					BlowFish cipher = cipherKey;

					std::vector<U8> bytes = FsUtils::ReadBinary(inputFile.string());

					cipher.Encrypt(bytes);

					FsUtils::CreateIfNotExist(outputFile);
					FsUtils::WriteBinary(outputFile, bytes);
				}
			}
			else
			{
				LOG("\n");
				LOG("Input file %s does not exist\n", inputFile.string().c_str());
				LOG("\n");
			}
		}
	}

	if ((Argc >= 4) && (std::strcmp(Argv[1], "Unpack") == 0))
	{
		fs::path inputFile = Argv[2];
		fs::path outputFile = Argv[3];

		if (fs::exists(inputFile))
		{
			if (fs::exists(outputFile))
			{
				LOG("\n");
				LOG("Output file %s already exists\n", outputFile.string().c_str());
				LOG("\n");
			}
			else
			{
				std::vector<U8> bytes = FsUtils::ReadBinary(inputFile);

				FsUtils::CreateIfNotExist(outputFile, true);

				Archive archive = bytes;

				archive.Load();
				archive.DumpToDiskRecursive(outputFile);
			}
		}
		else
		{
			LOG("\n");
			LOG("Input file %s does not exist\n", inputFile.string().c_str());
			LOG("\n");
		}
	}

	if ((Argc >= 3) && (std::strcmp(Argv[1], "ToC") == 0))
	{
		fs::path inputFile = Argv[2];

		if (fs::exists(inputFile))
		{
			std::vector<U8> bytes = FsUtils::ReadBinary(inputFile);

			Archive archive = bytes;

			archive.Load();

			LOG("\n");
			LOG(" Table Of Content For %s\n", inputFile.filename().string().c_str());
			LOG("=============================================================\n");

			archive.DumpTableOfContent(0, 4, 4);

			LOG("\n");
		}
		else
		{
			LOG("\n");
			LOG("Input file %s does not exist\n", inputFile.string().c_str());
			LOG("\n");
		}
	}

	return 0;
}
