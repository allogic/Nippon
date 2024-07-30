#include <Common/Memory.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Model/Model.h>

#include <Common/Utility/FileUtility.h>

using namespace Nippon;

// TODO
// PrintToC "C:\Users\mialb\Downloads\Nippon\x64\Debug\r301\r301.SCP\14_minka1_house_scr.SCR"
// ConvertIntoProprietaryFormat "C:\Users\mialb\Downloads\Nippon\x64\Debug\Monkey.fbx" "C:\Users\mialb\Downloads\Nippon\x64\Debug\Monkey.SCR"
// Test "C:\Users\mialb\Downloads\Nippon\x64\Debug\r301\r301.SCP\14_minka1_house_scr.SCR" "C:\Users\mialb\Downloads\Nippon\x64\Debug\Minka.SCR"

I32 main(I32 Argc, char** Argv)
{
	if (std::strcmp("PrintToC", Argv[1]) == 0)
	{
		std::vector<U8> modelData = FileUtility::ReadBinary(Argv[2]);

		Model model;

		model.Deserialize(modelData);
		model.PrintTableOfContent([](char const* Message) { std::printf(Message); });
	}

	if (std::strcmp("ConvertIntoProprietaryFormat", Argv[1]) == 0)
	{
		Model model;

		bool converted = model.ConvertIntoProprietaryFormat(Argv[2]);

		if (converted)
		{
			std::vector<U8> modelData = model.Serialize();

			FileUtility::WriteBinary(Argv[3], modelData);

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

	if (std::strcmp("Test", Argv[1]) == 0)
	{
		std::vector<U8> modelData = FileUtility::ReadBinary(Argv[2]);

		Model model;

		model.Deserialize(modelData);
		
		modelData = model.Serialize();

		FileUtility::WriteBinary(Argv[3], modelData);
	}

	if (std::strcmp("Help", Argv[1]) == 0)
	{
		std::printf("\n");
		std::printf("Usage:\n");
		std::printf("\n");
		std::printf(" ModelUtility [Command] [Arguments]\n");
		std::printf("\n");
		std::printf("Commands:\n");
		std::printf(" PrintToC                     [Model(Str)]             Print the table of content for SCP/MD files\n");
		std::printf(" ConvertIntoProprietaryFormat [File(Str)] [Model(Str)] Convert a standardized 3D file into the internal proprietary format\n");
		std::printf(" Help                                                  Print this help message\n");
		std::printf("\n");
		std::printf("Examples:\n");
		std::printf(" PrintToC \"minka.SCR\"\n");
		std::printf(" ConvertIntoProprietaryFormat \"monkey.FBX\" \"monkey.SCR\"\n");
		std::printf("\n");
	}

	Memory::CheckForLeaks();

	return 0;
}