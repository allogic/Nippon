#include <cstring>
#include <vector>
#include <cmath>

#include <Common/Debug.h>
#include <Common/Types.h>

#include <Common/Utils/FileUtils.h>

ark::I32 main(ark::I32 Argc, char** Argv)
{
  LOG("\n");

  if (std::strcmp(Argv[1], "Compare") == 0)
  {
    std::vector<ark::U8> bytesLeft = ark::FileUtils::ReadBinary(Argv[2]);
    std::vector<ark::U8> bytesRight = ark::FileUtils::ReadBinary(Argv[3]);

    LOG("BytesLeft:  %10llu\n", bytesLeft.size());
    LOG("BytesRight: %10llu\n", bytesRight.size());
    LOG("\n");

    ark::U64 byteSize = std::min(bytesLeft.size(), bytesRight.size());

    for (ark::U64 i = 0; i < byteSize; i++)
    {
      if (bytesLeft[i] != bytesRight[i])
      {
        LOG("Missmatch at 0x%llX\n", i);

        break;
      }
    }
  }

  if (std::strcmp(Argv[1], "Search") == 0)
  {
    std::vector<ark::U8> bytes = ark::FileUtils::ReadBinary(Argv[2]);
    std::string pattern = Argv[3];

    for (ark::U64 i = 0; i < bytes.size(); i++)
    {
      bool found = true;

      for (ark::U32 j = 0; j < pattern.size(); j++)
      {
        if (bytes[i + j] != pattern[j])
        {
          found = false;

          break;
        }
      }

      if (found)
      {
        LOG("Found at 0x%llX\n", i);

        i += pattern.size();
      }
    }
  }

  LOG("\n");

  return 0;
}