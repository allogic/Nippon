#include <Common/Alignment.h>

#include <Common/Utils/FileUtils.h>

#include <Editor/Scene.h>
#include <Editor/Vertex.h>

#include <Editor/Serializer/ModelSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ModelSerializer::ModelSerializer(Scene* Scene, const fs::path& File)
    : mFile{ File }
    , mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    U64 scrStart = mBinaryReader.GetPosition();

    ScrHeader scrHeader = mBinaryReader.Read<ScrHeader>();

    assert(scrHeader.ScrId == 0x00726373);

    std::vector<U32> transformOffsets = mBinaryReader.Read<U32>(scrHeader.SubMeshCount);

    mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));

    ModelGroup modelGroup = { mFile.stem().string() };

    for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
    {
      ParseModel(modelGroup);

      mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));
    }

    for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
    {
      mBinaryReader.SeekAbsolute(scrStart + transformOffsets[i]);

      ScrTransform scrTransform = mBinaryReader.Read<ScrTransform>();

      ModelEntry& modelEntry = modelGroup[i];

      modelEntry.SetPosition(R32V3{ scrTransform.Position.x, scrTransform.Position.y, scrTransform.Position.z });
      modelEntry.SetRotation(R32V3{ scrTransform.Rotation.x, scrTransform.Rotation.y, scrTransform.Rotation.z });
      modelEntry.SetScale(R32V3{ scrTransform.Scale.x, scrTransform.Scale.y, scrTransform.Scale.z });
    }

    Scene->AddModelGroup(modelGroup);
  }

  void ModelSerializer::ParseModel(ModelGroup& ModelGroup)
  {
    U64 mdbStart = mBinaryReader.GetPosition();

    MdbHeader mdbHeader = mBinaryReader.Read<MdbHeader>();

    assert(mdbHeader.MdbId == 0x0062646D);

    ModelEntry modelEntry = { mdbHeader.MeshId, mdbHeader.MeshType };

    std::vector<U32> divisionOffsets = mBinaryReader.Read<U32>(mdbHeader.MeshDivisions);

    for (U16 i = 0; i < mdbHeader.MeshDivisions; i++)
    {
      mBinaryReader.SeekAbsolute(mdbStart + divisionOffsets[i]);

      ModelDivision modelDivision = {};

      ParseDivision(modelDivision);

      modelEntry.AddDivision(modelDivision);
    }

    ModelGroup.AddEntry(modelEntry);
  }

  void ModelSerializer::ParseDivision(ModelDivision& ModelDivision)
  {
    U64 mdStart = mBinaryReader.GetPosition();

    MdHeader mdHeader = mBinaryReader.Read<MdHeader>();

    std::vector<ScrVertex> vertices = {};
    std::vector<U16V2> textureMaps = {};
    std::vector<U16V2> textureUvs = {};
    std::vector<U32> colorWeights = {};
    std::vector<U32> elements = {};

    if (mdHeader.VertexOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.VertexOffset);
      mBinaryReader.Read<ScrVertex>(vertices, mdHeader.VertexCount);
    }

    if (mdHeader.TextureMapOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureMapOffset);
      mBinaryReader.Read<U16V2>(textureMaps, mdHeader.VertexCount);
    }

    if (mdHeader.TextureUvOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureUvOffset);
      mBinaryReader.Read<U16V2>(textureUvs, mdHeader.VertexCount);
    }

    if (mdHeader.ColorWeightOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.ColorWeightOffset);
      mBinaryReader.Read<U32>(colorWeights, mdHeader.VertexCount);
    }

    if (mdHeader.VertexCount >= 3)
    {
      for (U16 i = 2; i < mdHeader.VertexCount; i++)
      {
        if (vertices[i].Connection == 0x8000)
        {
          continue;
        }
    
        elements.emplace_back(i - 2);
        elements.emplace_back(i - 1);
        elements.emplace_back(i - 0);
      }
    }

    for (U16 i = 0; i < mdHeader.VertexCount; i++)
    {
      R32V3 position = (mdHeader.VertexCount == vertices.size()) ? R32V3{ vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z } : R32V3{};
      R32V2 textureMap = (mdHeader.VertexCount == textureMaps.size()) ? R32V2{ 1000.0f / textureMaps[i].x, 1000.0f / textureMaps[i].y } : R32V2{};
      R32V2 textureUv = (mdHeader.VertexCount == textureUvs.size()) ? R32V2{ textureUvs[i].x, textureUvs[i].y } : R32V2{};
      U32 colorWeight = (mdHeader.VertexCount == colorWeights.size()) ? colorWeights[i] : 0;

      ModelDivision.AddVertex(DefaultVertex{ position, textureMap, textureUv, colorWeight });
    }

    for (U16 i = 0; i < elements.size(); i++)
    {
      ModelDivision.AddElement(elements[i]);
    }
  }
}
