#include <Common/Alignment.h>

#include <Common/Utils/FileUtils.h>

#include <Editor/AssetDatabase.h>
#include <Editor/Vertex.h>

#include <Editor/Serializer/ModelSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ModelSerializer::ModelSerializer(const fs::path& File)
    : mFile{ File }
    , mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    ParseScr();
  }

  void ModelSerializer::ParseScr()
  {
    U64 scrStart = mBinaryReader.GetPosition();

    ScrHeader scrHeader = mBinaryReader.Read<ScrHeader>();

    assert(scrHeader.ScrId == 0x00726373);

    std::vector<U32> transformOffsets = mBinaryReader.Read<U32>(scrHeader.EndEntryCount);

    mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));

    Model model;

    model.SetName(mFile.stem().string());

    for (U32 i = 0; i < 1; i++)
    {
      ParseModel(model);

      mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));
    }

    for (U32 i = 0; i < 1; i++)
    {
      mBinaryReader.SeekAbsolute(scrStart + transformOffsets[i]);

      ScrTransform scrTransform = mBinaryReader.Read<ScrTransform>();

      model.SetTransform(Model::Transform{
        R32V3{ scrTransform.Position.x, scrTransform.Position.y, scrTransform.Position.z },
        R32V3{ scrTransform.Rotation.x, scrTransform.Rotation.y, scrTransform.Rotation.z },
        R32V3{ scrTransform.Scale.x, scrTransform.Scale.y, scrTransform.Scale.z },
        });
    }

    AssetDatabase::AddModel(model);
  }

  void ModelSerializer::ParseModel(Model& Model)
  {
    U64 mdbStart = mBinaryReader.GetPosition();

    MdbHeader mdbHeader = mBinaryReader.Read<MdbHeader>();

    assert(mdbHeader.MdbId == 0x0062646D);

    std::vector<U32> divisionOffsets = mBinaryReader.Read<U32>(mdbHeader.MeshDivisions);

    for (U16 i = 0; i < 1; i++)
    {
      mBinaryReader.SeekAbsolute(mdbStart + divisionOffsets[i]);

      std::vector<ScrVertex> vertices;
      std::vector<U16V2> textureMaps;
      std::vector<U16V2> textureUvs;
      std::vector<U32> colorWeights;
      std::vector<U32> elements;

      auto [vertexCount, elementCount] = ParseSubModel(vertices, textureMaps, textureUvs, colorWeights, elements);

      for (U16 i = 0; i < vertexCount; i++)
      {
        R32V3 position = (vertexCount == vertices.size()) ? R32V3{ vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z } : R32V3{};
        R32V2 textureMap = (vertexCount == textureMaps.size()) ? R32V2{ textureMaps[i].x, textureMaps[i].y } : R32V2{};
        R32V2 textureUv = (vertexCount == textureUvs.size()) ? R32V2{ textureUvs[i].x, textureUvs[i].y } : R32V2{};
        U32 colorWeight = (vertexCount == colorWeights.size()) ? colorWeights[i] : U32{};

        Model.AddVertex(DefaultVertex{ position, textureMap, textureUv, colorWeight });
      }

      for (U16 i = 0; i < elementCount; i++)
      {
        Model.AddElement(elements[i]);
      }
    }
  }

  std::pair<U16, U16> ModelSerializer::ParseSubModel(std::vector<ScrVertex>& Vertices, std::vector<U16V2>& TextureMaps, std::vector<U16V2>& TextureUvs, std::vector<U32>& ColorWeights, std::vector<U32>& Elements)
  {
    U64 mdStart = mBinaryReader.GetPosition();

    MdHeader mdHeader = mBinaryReader.Read<MdHeader>();

    if (mdHeader.VertexOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.VertexOffset);
      mBinaryReader.Read<ScrVertex>(Vertices, mdHeader.VertexCount);
    }

    if (mdHeader.TextureMapOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureMapOffset);
      mBinaryReader.Read<U16V2>(TextureMaps, mdHeader.VertexCount);
    }

    if (mdHeader.TextureUvOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureUvOffset);
      mBinaryReader.Read<U16V2>(TextureUvs, mdHeader.VertexCount);
    }

    if (mdHeader.ColorWeightOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.ColorWeightOffset);
      mBinaryReader.Read<U32>(ColorWeights, mdHeader.VertexCount);
    }

    if (mdHeader.VertexCount >= 3)
    {
      for (U16 i = 2; i < mdHeader.VertexCount; i++)
      {
        if (Vertices[i].Connection == 0x8000)
        {
          continue;
        }
    
        Elements.emplace_back(i - 2);
        Elements.emplace_back(i - 1);
        Elements.emplace_back(i - 0);
      }
    }

    return { mdHeader.VertexCount, (U16)Elements.size() };
  }
}
