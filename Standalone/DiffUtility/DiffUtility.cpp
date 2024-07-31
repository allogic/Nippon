#include <Common/Version.h>
#include <Common/Memory.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Model/Model.h>

#include <Common/Utility/FileUtility.h>

using namespace Nippon;

I32 main(I32 Argc, char** Argv)
{
	if (std::strcmp("Compare", Argv[1]) == 0)
	{
		std::vector<U8> leftData = {};
		std::vector<U8> rightData = {};

		FileUtility::ReadBinary(Argv[2], leftData);
		FileUtility::ReadBinary(Argv[3], rightData);

		U64 size = std::min(leftData.size(), rightData.size());
		U64 position = 0;

		bool identical = true;

		for (U64 i = 0; i < size; i++)
		{
			if (leftData[i] != rightData[i])
			{
				position = i;
				identical = false;

				break;
			}
		}

		if (identical)
		{
			std::printf("\n");
			std::printf("Done\n");
			std::printf("\n");
		}
		else
		{
			std::printf("\n");
			std::printf("Mismatch at 0x%016llX\n", position);
			std::printf("\n");
		}
	}

	if (std::strcmp("Version", Argv[1]) == 0)
	{
		std::printf("\n");
		std::printf("Version %s\n", NIPPON_VERSION_STR);
		std::printf("\n");
	}

	if (std::strcmp("Help", Argv[1]) == 0)
	{
		std::printf("\n");
		std::printf("Usage:\n");
		std::printf("\n");
		std::printf(" DiffUtility [Command] [Arguments]\n");
		std::printf("\n");
		std::printf("Commands:\n");
		std::printf(" Compare [Left(Str)] [Right(Str)] Compare two files and output the exact position at which they separate\n");
		std::printf(" Version                          Print the current version\n");
		std::printf(" Help                             Print this help message\n");
		std::printf("\n");
		std::printf("Examples:\n");
		std::printf(" Compare \"r301.dat\" \"r302.dat\"\n");
		std::printf("\n");
	}

	Memory::CheckForLeaks();

	return 0;
}