#include <Common/Version.h>
#include <Common/Memory.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Archive/Archive.h>

#include <Common/Misc/BlowFish.h>

#include <Common/Utility/FileUtility.h>

using namespace Nippon;

I32 main(I32 Argc, char** Argv)
{
	if (std::strcmp("PrintToC", Argv[1]) == 0)
	{
		std::vector<U8> archiveData = {};

		FileUtility::ReadBinary(Argv[2], archiveData);

		BlowFish::Create(OKAMI_CIPHER_KEY);
		BlowFish::Decrypt(archiveData);

		Archive archive;

		archive.Deserialize(archiveData);
		archive.PrintTableOfContent([](char const* Message) { std::printf(Message); });
	}

	if (std::strcmp("PrintOfType", Argv[1]) == 0)
	{
		std::vector<U8> archiveData = {};

		FileUtility::ReadBinary(Argv[2], archiveData);

		BlowFish::Create(OKAMI_CIPHER_KEY);
		BlowFish::Decrypt(archiveData);

		Archive archive;

		archive.Deserialize(archiveData);
		archive.PrintOfType([](char const* Message) { std::printf(Message); }, Argv[3]);
	}

	if (std::strcmp("Extract", Argv[1]) == 0)
	{
		std::vector<U8> archiveData = {};

		FileUtility::ReadBinary(Argv[2], archiveData);

		BlowFish::Create(OKAMI_CIPHER_KEY);
		BlowFish::Decrypt(archiveData);

		Archive archive;

		archive.Deserialize(archiveData);
		archive.ExtractToDisk(Argv[3]);

		std::printf("\n");
		std::printf("Done\n");
		std::printf("\n");
	}

	if (std::strcmp("Unfold", Argv[1]) == 0)
	{
		std::vector<U8> archiveData = {};

		FileUtility::ReadBinary(Argv[2], archiveData);

		BlowFish::Create(OKAMI_CIPHER_KEY);
		BlowFish::Decrypt(archiveData);

		Archive archive;

		archive.Deserialize(archiveData);
		archive.UnfoldToDisk(Argv[3]);

		std::printf("\n");
		std::printf("Done\n");
		std::printf("\n");
	}

	if (std::strcmp("GetByTypeAndName", Argv[1]) == 0)
	{
		std::vector<U8> archiveData = {};

		FileUtility::ReadBinary(Argv[2], archiveData);

		BlowFish::Create(OKAMI_CIPHER_KEY);
		BlowFish::Decrypt(archiveData);

		Archive archive;

		archive.Deserialize(archiveData);

		if (Archive* targetArchive = archive.FindArchiveByTypeAndName(Argv[3], Argv[4]))
		{
			FileUtility::WriteBinary(Argv[5], targetArchive->GetBytes(), targetArchive->GetSize());

			std::printf("\n");
			std::printf("Done\n");
			std::printf("\n");
		}
		else
		{
			std::printf("\n");
			std::printf("Cannot find archive\n");
			std::printf("\n");
		}
	}

	if (std::strcmp("SetByTypeAndName", Argv[1]) == 0)
	{
		std::vector<U8> archiveData = {};
		std::vector<U8> targetData = {};

		FileUtility::ReadBinary(Argv[2], archiveData);
		FileUtility::ReadBinary(Argv[5], targetData);

		BlowFish::Create(OKAMI_CIPHER_KEY);
		BlowFish::Decrypt(archiveData);

		Archive archive;

		archive.Deserialize(archiveData);

		if (Archive* targetArchive = archive.FindArchiveByTypeAndName(Argv[3], Argv[4]))
		{
			targetArchive->SetData(targetData.data(), targetData.size());

			archiveData = archive.Serialize();

			BlowFish::Encrypt(archiveData);

			FileUtility::WriteBinary(Argv[2], archiveData);

			std::printf("\n");
			std::printf("Done\n");
			std::printf("\n");
		}
		else
		{
			std::printf("\n");
			std::printf("Cannot find archive\n");
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
		std::printf(" ArchiveUtility [Command] [Arguments]\n");
		std::printf("\n");
		std::printf("Commands:\n");
		std::printf(" PrintToC         [Archive(Str)]                                     Print the table of content for AKT/BIN/CMP/DAT/DDP/EFF/IDD/PAC/SCP/TBL files\n");
		std::printf(" PrintOfType      [Archive(Str)] [Type(Str)]                         Print all entries of type\n");
		std::printf(" Extract          [Archive(Str)] [Folder(Str)]                       Extract an archive to disk\n");
		std::printf(" Unfold           [Archive(Str)] [Folder(Str)]                       Unfold an archive to disk\n");
		std::printf(" GetByTypeAndName [Archive(Str)] [Type(Str)] [Name(Str)] [File(Str)] Get an archive entry identified by type and name into a file\n");
		std::printf(" SetByTypeAndName [Archive(Str)] [Type(Str)] [Name(Str)] [File(Str)] Set an archive entry identified by type and name from a file\n");
		std::printf(" Version                                                             Print the current version\n");
		std::printf(" Help                                                                Print this help message\n");
		std::printf("\n");
		std::printf("Examples:\n");
		std::printf(" PrintToC \"r301.dat\"\n");
		std::printf(" PrintOfType \"r301.dat\" \"DDS\"\n");
		std::printf(" Extract \"r301.dat\" \"r301\"\n");
		std::printf(" Unfold \"r301.dat\" \"r301\"\n");
		std::printf(" GetByTypeAndName \"r301.dat\" \"DDS\" \"hyouzan\" \"hyouzan.DDS\"\n");
		std::printf(" SetByTypeAndName \"r301.dat\" \"DDS\" \"hyouzan\" \"hyouzan.DDS\"\n");
		std::printf("\n");
	}

	Memory::CheckForLeaks();

	return 0;
}