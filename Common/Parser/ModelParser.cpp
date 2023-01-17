#include <vector>
#include <cassert>

#include <Common/Alignment.h>

#include <Common/Parser/ModelParser.h>

#include <Common/Utils/FileUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ModelParser::ModelParser(const fs::path& File)
    : mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    std::printf("%s\n", File.string().c_str());

    ParseScr();

    std::printf("\n");
  }

  void ModelParser::ParseScr()
  {
    U64 scrStart = mBinaryReader.GetPosition();

    ScrHeader scrHeader = mBinaryReader.Read<ScrHeader>();

    assert(scrHeader.ScrId == 0x00726373);

    std::vector<U32> transformOffsets = mBinaryReader.Read<U32>(scrHeader.EndEntryCount);

    mBinaryReader.SeekAbsolute(Align16(mBinaryReader.GetPosition()));

    for (U32 i = 0; i < 1; i++)
    {
      ParseMdb();

      mBinaryReader.SeekAbsolute(Align16(mBinaryReader.GetPosition()));
    }

    for (U32 i = 0; i < scrHeader.EndEntryCount; i++)
    {
      mBinaryReader.SeekAbsolute(scrStart + transformOffsets[i]);

      mTransforms.emplace_back(mBinaryReader.Read<ScrTransform>());
    }
  }

  void ModelParser::ParseMdb()
  {
    U64 mdbStart = mBinaryReader.GetPosition();

    MdbHeader mdbHeader = mBinaryReader.Read<MdbHeader>();

    assert(mdbHeader.MdbId == 0x0062646D);

    std::vector<U32> divisionOffsets = mBinaryReader.Read<U32>(mdbHeader.MeshDivisions);

    for (U16 i = 0; i < mdbHeader.MeshDivisions; i++)
    {
      mBinaryReader.SeekAbsolute(mdbStart + divisionOffsets[i]);

      ParseMd();
    }
  }

  void ModelParser::ParseMd()
  {
    MdHeader mdHeader = mBinaryReader.Read<MdHeader>();

    if (mdHeader.Unknown1 != 0)
    {
      return;
    }

    std::printf("\tVertexCount:%u\n", mdHeader.VertexCount);
  }
}
