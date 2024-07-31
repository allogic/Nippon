#include <Common/Version.h>
#include <Common/Memory.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Model/Model.h>

#include <Common/Utility/FileUtility.h>

using namespace Nippon;

I32 main(I32 Argc, char** Argv)
{
	if (std::strcmp("PrintToC", Argv[1]) == 0)
	{
		std::vector<U8> modelData = {};

		FileUtility::ReadBinary(Argv[2], modelData);

		Model model;

		model.Deserialize(modelData);
		model.PrintTableOfContent([](char const* Message) { std::printf(Message); });
	}

	if (std::strcmp("ConvertIntoProprietaryFormat", Argv[1]) == 0)
	{
		Model model;

		bool converted = model.ConvertIntoProprietaryFormat(Argv[2], Argv[3]);

		if (converted)
		{
			std::vector<U8> modelData = model.Serialize();

			FileUtility::WriteBinary(Argv[4], modelData);

			std::printf("\n");
			std::printf("Done\n");
			std::printf("\n");
		}
		else
		{
			std::printf("\n");
			std::printf("Cannot convert model\n");
			std::printf("\n");
		}
	}

	// TODO
	if (std::strcmp("Test", Argv[1]) == 0)
	{
		std::vector<U8> modelData = {};

		FileUtility::ReadBinary(Argv[2], modelData);

		Model model;

		model.Deserialize(modelData);
		
		modelData = model.Serialize();

		FileUtility::WriteBinary(Argv[3], modelData);
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
		std::printf(" ModelUtility [Command] [Arguments]\n");
		std::printf("\n");
		std::printf("Commands:\n");
		std::printf(" PrintToC                     [Model(Str)]                          Print the table of content for SCR/MD files\n");
		std::printf(" ConvertIntoProprietaryFormat [File(Str)] [Rules(Str)] [Model(Str)] Convert a standardized 3D file into the internal proprietary format\n");
		std::printf(" Version                                                            Print the current version\n");
		std::printf(" Help                                                               Print this help message\n");
		std::printf("\n");
		std::printf("Examples:\n");
		std::printf(" PrintToC \"minka.SCR\"\n");
		std::printf(" ConvertIntoProprietaryFormat \"monkey.FBX\" \"monkeyRules.JSON\" \"monkey.SCR\"\n");
		std::printf("\n");
	}

	Memory::CheckForLeaks();

	return 0;
}