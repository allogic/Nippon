#include <string>
#include <filesystem>
#include <vector>

#include <Common/Macros.h>
#include <Common/Types.h>
#include <Common/CommandLine.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

using namespace ark;

namespace fs = std::filesystem;

I32 main(I32 Argc, char** Argv)
{
	CommandLine::Init(Argc, Argv);

	if (CommandLine::HasFirstArgument("Help"))
	{
		LOG("\n");
		LOG("ArchiveAnalyzer Decrypt Input Output\n");
		LOG("ArchiveAnalyzer Encrypt Input Output\n");
		LOG("ArchiveAnalyzer Compress Input Output [Decrypt|Encrypt] (Experimental)\n");
		LOG("ArchiveAnalyzer Unfold Input Output [Decrypt]\n");
		LOG("ArchiveAnalyzer Extract Input Output [Decrypt]\n");
		LOG("ArchiveAnalyzer ToC Input [Decrypt|Offset|Increment]\n");
		LOG("\n");
	}

	if (CommandLine::HasFirstArgument("Decrypt"))
	{
		std::string inputFile = "";
		std::string outputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("Output", outputFile, "Output file is missing"))
			{
				if (fs::exists(inputFile))
				{
					std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

					BlowFish{ CIPHER_KEY }.Decrypt(inputBytes);

					FsUtils::CreateDirIfNotExist(outputFile);
					FsUtils::WriteBinary(outputFile, inputBytes);
				}
			}
		}
	}

	if (CommandLine::HasFirstArgument("Encrypt"))
	{
		std::string inputFile = "";
		std::string outputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("Output", outputFile, "Output file is missing"))
			{
				if (fs::exists(inputFile))
				{
					std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

					BlowFish{ CIPHER_KEY }.Encrypt(inputBytes);

					FsUtils::CreateDirIfNotExist(outputFile);
					FsUtils::WriteBinary(outputFile, inputBytes);
				}
			}
		}
	}

	if (CommandLine::HasFirstArgument("Compress"))
	{
		std::string inputFile = "";
		std::string outputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("Output", outputFile, "Output file is missing"))
			{
				if (fs::exists(inputFile))
				{
					std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

					if (CommandLine::HasArgument("Decrypt"))
					{
						BlowFish{ CIPHER_KEY }.Decrypt(inputBytes);
					}

					Archive archive = {};

					archive.DeSerialize(inputBytes);

					std::vector<U8> outputBytes = archive.Serialize();

					if (CommandLine::HasArgument("Encrypt"))
					{
						BlowFish{ CIPHER_KEY }.Encrypt(outputBytes);
					}

					FsUtils::CreateDirIfNotExist(outputFile);
					FsUtils::WriteBinary(outputFile, outputBytes);
				}
			}
		}
	}

	if (CommandLine::HasFirstArgument("Unfold"))
	{
		std::string inputFile = "";
		std::string outputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("Output", outputFile, "Output file is missing"))
			{
				if (fs::exists(inputFile))
				{
					std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

					if (CommandLine::HasArgument("Decrypt"))
					{
						BlowFish{ CIPHER_KEY }.Decrypt(inputBytes);
					}

					FsUtils::CreateDirIfNotExist(outputFile, true);

					Archive archive = {};

					archive.DeSerialize(inputBytes);
					archive.UnfoldToDisk(outputFile);
				}
			}
		}
	}

	if (CommandLine::HasFirstArgument("Extract"))
	{
		std::string inputFile = "";
		std::string outputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("Output", outputFile, "Output file is missing"))
			{
				if (fs::exists(inputFile))
				{
					std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

					if (CommandLine::HasArgument("Decrypt"))
					{
						BlowFish{ CIPHER_KEY }.Decrypt(inputBytes);
					}

					FsUtils::CreateDirIfNotExist(outputFile, true);

					Archive archive = {};

					archive.DeSerialize(inputBytes);
					archive.ExtractToDisk(outputFile);
				}
			}
		}
	}

	if (CommandLine::HasFirstArgument("ToC"))
	{
		std::string inputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (fs::exists(inputFile))
			{
				std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

				if (CommandLine::HasArgument("Decrypt"))
				{
					BlowFish{ CIPHER_KEY }.Decrypt(inputBytes);
				}

				Archive archive = {};

				archive.DeSerialize(inputBytes);

				U32 offset = 0;
				U32 increment = 4;

				std::string offsetStr = "";
				std::string incrementStr = "";

				if (CommandLine::HasArgumentWithValue("Offset", offsetStr))
				{
					offset = std::atoi(offsetStr.c_str());
				}

				if (CommandLine::HasArgumentWithValue("Increment", incrementStr))
				{
					increment = std::atoi(incrementStr.c_str());
				}

				archive.PrintTableOfContent(offset, 0, increment);
			}
		}
	}

	// TODO: Remove this function afterwards!

	if (CommandLine::HasFirstArgument("CheckAlignment"))
	{
		std::string inputFile = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (fs::exists(inputFile))
			{
				std::vector<U8> originalBytes = FsUtils::ReadBinary(inputFile);

				if (CommandLine::HasArgument("Decrypt"))
				{
					BlowFish{ CIPHER_KEY }.Decrypt(originalBytes);
				}

				Archive archive = {};

				archive.DeSerialize(originalBytes);

				U32 v0 = 0;
				U32 v1 = 0;

				std::string v0Str = "";
				std::string v1Str = "";

				if (CommandLine::HasArgumentWithValue("V0", v0Str))
				{
					v0 = std::atoi(v0Str.c_str());
				}

				if (CommandLine::HasArgumentWithValue("V1", v1Str))
				{
					v1 = std::atoi(v1Str.c_str());
				}

				std::vector<U8> modifiedBytes = archive.Serialize();

				std::vector<std::pair<U64, U64>> indices = {};

				if (DiffUtils::Compare(originalBytes, modifiedBytes, indices))
				{
					DiffUtils::HexDump(originalBytes, modifiedBytes, indices, 1, 256, 16, 12);
				}
				else
				{
					LOG("File matches %s\n", inputFile.c_str());
				}
			}
		}
	}

	return 0;
}
