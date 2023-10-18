#include <string>
#include <vector>
#include <set>

#include <Common/Macros.h>
#include <Common/Types.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

using namespace ark;

I32 main(I32 Argc, char** Argv)
{
	if (std::strcmp(Argv[1], "Help") == 0)
	{
		LOG("\n");
		LOG("ArchiveAnalyzer.exe Decrypt <cipher-key> <input-file> <output-file>\n");
		LOG("ArchiveAnalyzer.exe Unpack <input-file> <output-dir>\n");
		LOG("\n");
	}

	if (std::strcmp(Argv[1], "Decrypt") == 0)
	{
		BlowFish cipher = { Argv[2] };

		std::vector<U8> bytes = FsUtils::ReadBinary(Argv[3]);

		cipher.Decrypt(bytes);

		FsUtils::WriteBinary(Argv[4], bytes);
	}

	if (std::strcmp(Argv[1], "Unpack") == 0)
	{
		FsUtils::CreateIfNotExists(Argv[3]);

		std::vector<U8> bytes = FsUtils::ReadBinary(Argv[2]);

		Archive archive = nullptr;

		archive.LoadRecursive(bytes, 0, 0, 0, "", Argv[3], true, true);
		archive.DumpToDiskRecursive(Argv[3]);
	}

	if (std::strcmp(Argv[1], "CollectTypes") == 0)
	{
		std::set<std::string> dirExtTypes = {};
		std::set<std::string> fileExtTypes = {};
		std::vector<U8> bytes = FsUtils::ReadBinary(Argv[2]);

		Archive archive = nullptr;

		archive.LoadAndCollectExtTypesRecursive(dirExtTypes, fileExtTypes, bytes, 0, 0, 0, "", "", true, true);

		LOG("\n");
		LOG(" Found Directory Extensions\n");
		LOG("=============================================================\n");

		for (const auto& type : dirExtTypes)
		{
			LOG("    %s\n", type.c_str());
		}

		LOG("\n");
		LOG(" Found File Extensions\n");
		LOG("=============================================================\n");

		for (const auto& type : fileExtTypes)
		{
			LOG("    %s\n", type.c_str());
		}

		LOG("\n");
	}

	return 0;
}
