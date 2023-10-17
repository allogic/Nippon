#include <string>
#include <vector>

#include <Common/Macros.h>
#include <Common/Types.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

using namespace ark;

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
		std::vector<U8> bytesLeft = FsUtils::ReadBinary(Argv[2]);
		std::vector<U8> bytesRight = FsUtils::ReadBinary(Argv[3]);

		std::vector<std::pair<U64, U64>> indices = {};

		if (DiffUtils::Compare(bytesLeft, bytesRight, indices))
		{
			DiffUtils::HexDump(bytesLeft, bytesRight, indices);
		}
	}

	if (std::strcmp(Argv[1], "Search") == 0)
	{
		std::vector<U8> bytes = FsUtils::ReadBinary(Argv[2]);
		std::string pattern = Argv[3];

		std::vector<U64> indices = FsUtils::SearchBytesInFile(bytes, { pattern.begin(), pattern.end() });

		for (const auto index : indices)
		{
			LOG("Found at 0x%llX\n", index);
		}
	}

	return 0;
}
