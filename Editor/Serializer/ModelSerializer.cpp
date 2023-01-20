#include <string>
#include <map>
#include <cassert>

#include <Common/Alignment.h>

#include <Common/Utils/FileUtils.h>

#include <Editor/UI/AssetDatabase.h>

#include <Editor/Serializer/ModelSerializer.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern std::map<std::string, ark::UI*> gUserInterface;

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

    mBinaryReader.SeekAbsolute(Align16(mBinaryReader.GetPosition()));

    Model model;

    model.SetName(mFile.stem().string());

    for (U32 i = 0; i < 1; i++)
    {
      ParseModel(model);

      mBinaryReader.SeekAbsolute(Align16(mBinaryReader.GetPosition()));
    }

    for (U32 i = 0; i < 1; i++)
    {
      mBinaryReader.SeekAbsolute(scrStart + transformOffsets[i]);

      ScrTransform scrTransform = mBinaryReader.Read<ScrTransform>();

      model.SetTransform(Model::Transform{ scrTransform.Position, scrTransform.Rotation, scrTransform.Scale });
    }

    ((AssetDatabase*)gUserInterface["assetDatabase"])->AddModel(model);
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
      std::vector<PackedPair<U16>> textureMaps;
      std::vector<PackedPair<U16>> textureUvs;
      std::vector<U32> colorWeights;
      std::vector<U32> elements;

      auto [vertexCount, elementCount] = ParseSubModel(vertices, textureMaps, textureUvs, colorWeights, elements);

      for (U16 i = 0; i < vertexCount; i++)
      {
        PackedTuple<I16> position = (vertexCount == vertices.size()) ? vertices[i].Position : PackedTuple<I16>{};
        PackedPair<U16> textureMap = (vertexCount == textureMaps.size()) ? textureMaps[i] : PackedPair<U16>{};
        PackedPair<U16> textureUv = (vertexCount == textureUvs.size()) ? textureUvs[i] : PackedPair<U16>{};
        U32 colorWeight = (vertexCount == colorWeights.size()) ? colorWeights[i] : U32{};

        Model.AddVertex(Model::Vertex{ position, textureMap, textureUv, colorWeight });
      }

      for (U16 i = 0; i < elementCount; i++)
      {
        Model.AddElement(elements[i]);
      }
    }
  }

  std::pair<U16, U16> ModelSerializer::ParseSubModel(std::vector<ScrVertex>& Vertices, std::vector<PackedPair<U16>>& TextureMaps, std::vector<PackedPair<U16>>& TextureUvs, std::vector<U32>& ColorWeights, std::vector<U32>& Elements)
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
      mBinaryReader.Read<PackedPair<U16>>(TextureMaps, mdHeader.VertexCount);
    }

    if (mdHeader.TextureUvOffset != 0)
    {
      mBinaryReader.SeekAbsolute(mdStart + mdHeader.TextureUvOffset);
      mBinaryReader.Read<PackedPair<U16>>(TextureUvs, mdHeader.VertexCount);
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
