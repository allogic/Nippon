#include <vector>

#include <Common/Debug.h>
#include <Common/Types.h>

#include <Common/Utils/ByteUtils.h>
#include <Common/Utils/FileUtils.h>

ark::I32 main(ark::I32 Argc, char** Argv)
{
  LOG("\n");

  if (std::strcmp(Argv[1], "Compare") == 0)
  {
    std::vector<ark::U8> bytesLeft = ark::FileUtils::ReadBinary(Argv[2]);
    std::vector<ark::U8> bytesRight = ark::FileUtils::ReadBinary(Argv[3]);

    ark::U64 index = ark::ByteUtils::Compare(bytesLeft, bytesRight);

    if (index != 0)
    {
      LOG("Missmatch at 0x%llX\n", index);
    }
  }

  if (std::strcmp(Argv[1], "Search") == 0)
  {
    std::vector<ark::U8> bytes = ark::FileUtils::ReadBinary(Argv[2]);
    std::string pattern = Argv[3];

    std::vector<ark::U64> indices = ark::ByteUtils::Search(bytes, { pattern.begin(), pattern.end() });

    for (const auto index : indices)
    {
      LOG("Found at 0x%llX\n", index);
    }
  }

  LOG("\n");

  return 0;
}