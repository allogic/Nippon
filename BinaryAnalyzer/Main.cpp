#include <string>
#include <filesystem>
#include <vector>

#include <Common/Macros.h>
#include <Common/Types.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

using namespace ark;

namespace fs = std::filesystem;

I32 main(I32 Argc, char** Argv)
{
	if (std::strcmp(Argv[1], "Help") == 0)
	{
		LOG("\n");
		LOG("BinaryAnalyzer.exe Compare <input-file-left> <input-file-right>\n");
		LOG("BinaryAnalyzer.exe Search <input-file> <byte-pattern>\n");
		LOG("\n");
	}

	if (std::strcmp(Argv[1], "Compare") == 0)
	{
		fs::path inputFileLeft = Argv[2];
		fs::path inputFileRight = Argv[3];

		if (fs::exists(inputFileLeft) && fs::exists(inputFileRight))
		{
			std::vector<U8> bytesLeft = FsUtils::ReadBinary(inputFileLeft);
			std::vector<U8> bytesRight = FsUtils::ReadBinary(inputFileRight);

			std::vector<std::pair<U64, U64>> indices = {};

			if (DiffUtils::Compare(bytesLeft, bytesRight, indices))
			{
				DiffUtils::HexDump(bytesLeft, bytesRight, indices);
			}
		}
		else
		{
			LOG("\n");
			if (!fs::exists(inputFileLeft))
			{
				LOG("Input file %s does not exist\n", inputFileLeft.string().c_str());
			}
			if (!fs::exists(inputFileRight))
			{
				LOG("Input file %s does not exist\n", inputFileRight.string().c_str());
			}
			LOG("\n");
		}
	}

	if (std::strcmp(Argv[1], "Search") == 0)
	{
		fs::path inputFile = Argv[2];

		std::string bytePattern = Argv[3];

		if (fs::exists(inputFile))
		{
			if (bytePattern.empty())
			{
				LOG("\n");
				LOG("Byte pattern is empty\n", inputFile.string().c_str());
				LOG("\n");
			}
			else
			{
				std::vector<U8> bytes = FsUtils::ReadBinary(inputFile);

				std::vector<U64> indices = FsUtils::SearchBytesInFile(bytes, { bytePattern.begin(), bytePattern.end() });

				for (const auto index : indices)
				{
					LOG("Found at 0x%llX\n", index);
				}
			}
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
