#include <cassert>

#include <Common/Parser/ObjectTableParser.h>

#include <Common/Utils/FileUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ObjectTableParser::ObjectTableParser(const fs::path& File)
    : mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    U32 size = mBinaryReader.Read<U32>();

    for (U32 i = 0; i < size; i++)
    {
      mObjects.emplace_back(mBinaryReader.Read<ObjectTableEntry>());
    }
  }
}
