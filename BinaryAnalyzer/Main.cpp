#include <string>
#include <filesystem>
#include <vector>

#include <Common/Macros.h>
#include <Common/Types.h>
#include <Common/CommandLine.h>
#include <Common/BlowFish.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

using namespace ark;

namespace fs = std::filesystem;

I32 main(I32 Argc, I8** Argv)
{
	CommandLine::Init(Argc, Argv);

	if (CommandLine::HasFirstArgument("Help"))
	{
		LOG("\n");
		LOG("BinaryAnalyzer Compare LeftInput RightInput [LeftDecrypt|RightDecrypt|ResultCount|Size|Stride|LeadStrideMultiplier]\n");
		LOG("BinaryAnalyzer Search Input StringPattern [Decrypt]\n");
		LOG("\n");
	}

	if (CommandLine::HasFirstArgument("Compare"))
	{
		std::string leftInputFile = "";
		std::string rightInputFile = "";

		if (CommandLine::HasArgumentWithValue("LeftInput", leftInputFile, "Left input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("RightInput", rightInputFile, "Right input file is missing"))
			{
				if (fs::exists(leftInputFile) && fs::exists(rightInputFile))
				{
					std::vector<U8> leftBytes = FsUtils::ReadBinary(leftInputFile);
					std::vector<U8> rightBytes = FsUtils::ReadBinary(rightInputFile);

					if (CommandLine::HasArgument("LeftDecrypt"))
					{
						BlowFish{ CIPHER_KEY }.Decrypt(leftBytes);
					}

					if (CommandLine::HasArgument("RightDecrypt"))
					{
						BlowFish{ CIPHER_KEY }.Decrypt(rightBytes);
					}

					std::vector<std::pair<U64, U64>> indices = {};

					if (DiffUtils::Compare(leftBytes, rightBytes, indices))
					{
						U32 resultCount = 1;
						U32 size = 64;
						U32 stride = 16;
						U32 leadStrideMultiplier = 3;

						std::string resultCountStr = "";
						std::string sizeStr = "";
						std::string strideStr = "";
						std::string leadStrideMultiplierStr = "";

						if (CommandLine::HasArgumentWithValue("ResultCount", resultCountStr))
						{
							resultCount = std::atoi(resultCountStr.c_str());
						}

						if (CommandLine::HasArgumentWithValue("Size", sizeStr))
						{
							size = std::atoi(sizeStr.c_str());
						}

						if (CommandLine::HasArgumentWithValue("Stride", strideStr))
						{
							stride = std::atoi(strideStr.c_str());
						}

						if (CommandLine::HasArgumentWithValue("LeadStrideMultiplier", leadStrideMultiplierStr))
						{
							leadStrideMultiplier = std::atoi(leadStrideMultiplierStr.c_str());
						}

						DiffUtils::HexDump(leftBytes, rightBytes, indices, resultCount, size, stride, leadStrideMultiplier);
					}
				}
			}
		}
	}

	if (CommandLine::HasFirstArgument("Search"))
	{
		std::string inputFile = "";

		std::string strPattern = "";

		if (CommandLine::HasArgumentWithValue("Input", inputFile, "Input file is missing"))
		{
			if (CommandLine::HasArgumentWithValue("StringPattern", strPattern, "String pattern is missing"))
			{
				if (fs::exists(inputFile))
				{
					std::vector<U8> inputBytes = FsUtils::ReadBinary(inputFile);

					if (CommandLine::HasArgument("Decrypt"))
					{
						BlowFish{ CIPHER_KEY }.Decrypt(inputBytes);
					}

					std::vector<U64> indices = FsUtils::SearchStringsInFile(inputBytes, { strPattern.begin(), strPattern.end() });

					for (const auto index : indices)
					{
						LOG("Found at 0x%llX\n", index);
					}
				}
			}
		}
	}

	return 0;
}
