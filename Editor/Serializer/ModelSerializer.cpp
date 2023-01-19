#include <vector>
#include <cassert>

#include <Common/Alignment.h>

#include <Common/Utils/FileUtils.h>

#include <Editor/Scene.h>

#include <Editor/Serializer/ModelSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ModelSerializer::ModelSerializer(Scene* Scene, const fs::path& File)
    : mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    std::printf("%s\n", File.string().c_str());

    ParseScr();

    //for (const auto& model : mModels)
    //{
    //  std::printf("  Model\n");
    //
    //  for (const auto& subModel : model.SubModels)
    //  {
    //    std::printf("    SubModel\n");
    //    std::printf("      Vertices:%llu\n", subModel.Vertices.size());
    //    std::printf("      Elements:%llu\n", subModel.Elements.size());
    //    std::printf("      TextureMaps:%llu\n", subModel.TextureMaps.size());
    //    std::printf("      TextureUvs:%llu\n", subModel.TextureUvs.size());
    //    std::printf("      ColorWeights:%llu\n", subModel.ColorWeights.size());
    //  }
    //
    //  std::printf("\n");
    //}
    //
    //for (const auto& transform : mTransforms)
    //{
    //  std::printf("  Transform\n");
    //  std::printf("    Position:[%6d,%6d,%6d]\n", transform.Position.x, transform.Position.y, transform.Position.z);
    //  std::printf("    Rotation:[%6d,%6d,%6d]\n", transform.Rotation.x, transform.Rotation.y, transform.Rotation.z);
    //  std::printf("       Scale:[%6u,%6u,%6u]\n", transform.Scale.x, transform.Scale.y, transform.Scale.z);
    //}

    std::printf("\n");
  }

  void ModelSerializer::ParseScr()
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

      ScrTransform transform = mBinaryReader.Read<ScrTransform>();
    }
  }

  void ModelSerializer::ParseMdb()
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

  void ModelSerializer::ParseMd()
  {
    U64 mdStart = mBinaryReader.GetPosition();

    MdHeader mdHeader = mBinaryReader.Read<MdHeader>();

    if (mdHeader.VertexOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.VertexOffset);

      for (U16 i = 0; i < mdHeader.VertexCount; i++)
      {
        //SubModel.Vertices.emplace_back(mBinaryReader.Read<ScrVertex>());
      }
    }

    if (mdHeader.Unknown1 != 0)
    {
      return;
    }

    if (mdHeader.TextureMapOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureMapOffset);

      for (U16 i = 0; i < mdHeader.VertexCount; i++)
      {
        //SubModel.TextureMaps.emplace_back(mBinaryReader.Read<PackedPair<U16>>());
      }
    }

    if (mdHeader.TextureUvOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureUvOffset);

      for (U16 i = 0; i < mdHeader.VertexCount; i++)
      {
        //SubModel.TextureUvs.emplace_back(mBinaryReader.Read<PackedPair<U16>>());
      }
    }

    if (mdHeader.ColorWeightOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.ColorWeightOffset);

      for (U16 i = 0; i < mdHeader.VertexCount; i++)
      {
        //SubModel.ColorWeights.emplace_back(mBinaryReader.Read<U32>());
      }
    }

    //if (mdHeader.VertexCount >= 3)
    //{
    //  for (U16 i = 2; i < mdHeader.VertexCount; i++)
    //  {
    //    if (SubModel.Vertices[i].Connection == 0x8000)
    //    {
    //      continue;
    //    }
    //
    //    SubModel.Elements.emplace_back(i - 2);
    //    SubModel.Elements.emplace_back(i - 1);
    //    SubModel.Elements.emplace_back(i - 0);
    //  }
    //}
  }
}
