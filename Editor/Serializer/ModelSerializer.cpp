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
    : mScene{ Scene }
    , mFile{ File }
    , mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    U64 scrStart = mBinaryReader.GetPosition();

    ScrHeader scrHeader = mBinaryReader.Read<ScrHeader>();

    assert(scrHeader.ScrId == 0x00726373);

    if (scrHeader.FileType == 0)
    {
      //scrHeader.SubMeshCount = 1;

      ParseMd(scrStart, scrHeader);
    }
    else if (scrHeader.FileType == 1)
    {
      ParseScr(scrStart, scrHeader);
    }
  }

  void ModelSerializer::ParseScr(U64 ScrStart, ScrHeader& ScrHeader)
  {
    std::vector<U32> transformOffsets = mBinaryReader.Read<U32>(ScrHeader.SubMeshCount);

    mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));

    ModelGroup modelGroup = { mFile.stem().string() };

    for (U32 i = 0; i < ScrHeader.SubMeshCount; i++)
    {
      U64 mdbStart = mBinaryReader.GetPosition();

      MdbHeader mdbHeader = mBinaryReader.Read<MdbHeader>();

      assert(mdbHeader.MdbId == 0x0062646D);

      ParseScrModel(mdbStart, mdbHeader, modelGroup);

      mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));
    }

    for (U32 i = 0; i < ScrHeader.SubMeshCount; i++)
    {
      mBinaryReader.SeekAbsolute(ScrStart + transformOffsets[i]);

      ScrTransform scrTransform = mBinaryReader.Read<ScrTransform>();

      ModelEntry& modelEntry = modelGroup[i];

      modelEntry.SetPosition(R32V3{ scrTransform.Position.x, scrTransform.Position.y, scrTransform.Position.z });
      modelEntry.SetRotation(R32V3{ scrTransform.Rotation.x, scrTransform.Rotation.y, scrTransform.Rotation.z });
      modelEntry.SetScale(R32V3{ scrTransform.Scale.x, scrTransform.Scale.y, scrTransform.Scale.z });
    }

    mScene->AddModelGroup(modelGroup);
  }

  void ModelSerializer::ParseScrModel(U64 MdbStart, MdbHeader& MdbHeader, ModelGroup& ModelGroup)
  {
    ModelEntry modelEntry = { MdbHeader.MeshId, MdbHeader.MeshType };

    std::vector<U32> divisionOffsets = mBinaryReader.Read<U32>(MdbHeader.MeshDivisions);

    for (U16 i = 0; i < MdbHeader.MeshDivisions; i++)
    {
      mBinaryReader.SeekAbsolute(MdbStart + divisionOffsets[i]);

      ModelDivision modelDivision = {};

      U64 mdStart = mBinaryReader.GetPosition();

      MdHeader mdHeader = mBinaryReader.Read<MdHeader>();

      ParseScrDivision(mdStart, mdHeader, modelDivision);

      modelEntry.AddDivision(modelDivision);
    }

    ModelGroup.AddEntry(modelEntry);
  }

  void ModelSerializer::ParseScrDivision(U64 MdStart, MdHeader& MdHeader, ModelDivision& ModelDivision)
  {
    std::vector<ScrVertex> vertices = {};
    std::vector<U16V2> textureMaps = {};
    std::vector<U16V2> textureUvs = {};
    std::vector<U32> colorWeights = {};
    std::vector<U32> elements = {};

    if (MdHeader.VertexOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.VertexOffset);
      mBinaryReader.Read<ScrVertex>(vertices, MdHeader.VertexCount);
    }

    if (MdHeader.TextureMapOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.TextureMapOffset);
      mBinaryReader.Read<U16V2>(textureMaps, MdHeader.VertexCount);
    }

    if (MdHeader.TextureUvOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.TextureUvOffset);
      mBinaryReader.Read<U16V2>(textureUvs, MdHeader.VertexCount);
    }

    if (MdHeader.ColorWeightOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.ColorWeightOffset);
      mBinaryReader.Read<U32>(colorWeights, MdHeader.VertexCount);
    }

    //Begin Function CalculateSurfaceNormal(Input Triangle) Returns Vector
    //
    //Set Vector U to (Triangle.p2 minus Triangle.p1)
    //Set Vector V to (Triangle.p3 minus Triangle.p1)
    //
    //Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
    //Set Normal.y to (multiply U.z by V.x) minus (multiply U.x by V.z)
    //Set Normal.z to (multiply U.x by V.y) minus (multiply U.y by V.x)
    //
    //Returning Normal
    //
    //End Function

    if (MdHeader.VertexCount >= 3)
    {
      for (U16 i = 2; i < MdHeader.VertexCount; i++)
      {
        if (vertices[i].Connection == 0x8000)
        {
          continue;
        }
        else
        {
          if (vertices[i].Connection)
          {
            elements.emplace_back(i - 2);
            elements.emplace_back(i - 1);
            elements.emplace_back(i - 0);
          }
          else
          {
            elements.emplace_back(i - 1);
            elements.emplace_back(i - 2);
            elements.emplace_back(i - 0);
          }
        }
      }
    }

    for (U16 i = 0; i < MdHeader.VertexCount; i++)
    {
      R32V3 position = (MdHeader.VertexCount == vertices.size()) ? R32V3{ vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z } : R32V3{};
      R32V2 textureMap = (MdHeader.VertexCount == textureMaps.size()) ? R32V2{ 1000.0f / textureMaps[i].x, 10000.0f / textureMaps[i].y } : R32V2{};
      R32V2 textureUv = (MdHeader.VertexCount == textureUvs.size()) ? R32V2{ textureUvs[i].x, textureUvs[i].y } : R32V2{};
      U32 colorWeight = (MdHeader.VertexCount == colorWeights.size()) ? colorWeights[i] : 0;

      ModelDivision.AddVertex(DefaultVertex{ position, textureMap, textureUv, colorWeight });
    }

    for (U16 i = 0; i < elements.size(); i++)
    {
      ModelDivision.AddElement(elements[i]);
    }
  }

  void ModelSerializer::ParseMd(U64 ScrStart, ScrHeader& ScrHeader)
  {
    std::vector<U32> transformOffsets = mBinaryReader.Read<U32>(ScrHeader.SubMeshCount);

    mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));

    ModelGroup modelGroup = { mFile.stem().string() };

    for (U32 i = 0; i < ScrHeader.SubMeshCount; i++)
    {
      U64 mdbStart = mBinaryReader.GetPosition();

      MdbHeader mdbHeader = mBinaryReader.Read<MdbHeader>();

      assert(mdbHeader.MdbId == 0x0062646D);

      ParseMdModel(mdbStart, mdbHeader, modelGroup);

      mBinaryReader.SeekAbsolute(Align<16>::Up(mBinaryReader.GetPosition()));
    }

    for (U32 i = 0; i < ScrHeader.SubMeshCount; i++)
    {
      mBinaryReader.SeekAbsolute(ScrStart + transformOffsets[i]);

      ScrTransform scrTransform = mBinaryReader.Read<ScrTransform>();

      ModelEntry& modelEntry = modelGroup[i];

      modelEntry.SetPosition(R32V3{ scrTransform.Position.x, scrTransform.Position.y, scrTransform.Position.z });
      modelEntry.SetRotation(R32V3{ scrTransform.Rotation.x, scrTransform.Rotation.y, scrTransform.Rotation.z });
      modelEntry.SetScale(R32V3{ 1.0F, 1.0F, 1.0F });
    }

    mScene->AddModelGroup(modelGroup);
  }

  void ModelSerializer::ParseMdModel(U64 MdbStart, MdbHeader& MdbHeader, ModelGroup& ModelGroup)
  {
    ModelEntry modelEntry = { MdbHeader.MeshId, MdbHeader.MeshType };

    std::vector<U32> divisionOffsets = mBinaryReader.Read<U32>(MdbHeader.MeshDivisions);

    for (U16 i = 0; i < MdbHeader.MeshDivisions; i++)
    {
      mBinaryReader.SeekAbsolute(MdbStart + divisionOffsets[i]);

      ModelDivision modelDivision = {};

      U64 mdStart = mBinaryReader.GetPosition();

      MdHeader mdHeader = mBinaryReader.Read<MdHeader>();

      ParseMdDivision(mdStart, mdHeader, modelDivision);

      modelEntry.AddDivision(modelDivision);
    }

    ModelGroup.AddEntry(modelEntry);
  }

  void ModelSerializer::ParseMdDivision(U64 MdStart, MdHeader& MdHeader, ModelDivision& ModelDivision)
  {
    std::vector<ScrVertex> vertices = {};
    std::vector<U16V2> textureMaps = {};
    std::vector<U16V2> textureUvs = {};
    std::vector<U32> colorWeights = {};
    std::vector<U32> elements = {};

    if (MdHeader.VertexOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.VertexOffset);
      mBinaryReader.Read<ScrVertex>(vertices, MdHeader.VertexCount);
    }

    if (MdHeader.TextureMapOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.TextureMapOffset);
      mBinaryReader.Read<U16V2>(textureMaps, MdHeader.VertexCount);
    }

    if (MdHeader.TextureUvOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.TextureUvOffset);
      mBinaryReader.Read<U16V2>(textureUvs, MdHeader.VertexCount);
    }

    if (MdHeader.ColorWeightOffset != 0)
    {
      mBinaryReader.SeekAbsolute(MdStart + MdHeader.ColorWeightOffset);
      mBinaryReader.Read<U32>(colorWeights, MdHeader.VertexCount);
    }

    //Begin Function CalculateSurfaceNormal(Input Triangle) Returns Vector
    //
    //Set Vector U to (Triangle.p2 minus Triangle.p1)
    //Set Vector V to (Triangle.p3 minus Triangle.p1)
    //
    //Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
    //Set Normal.y to (multiply U.z by V.x) minus (multiply U.x by V.z)
    //Set Normal.z to (multiply U.x by V.y) minus (multiply U.y by V.x)
    //
    //Returning Normal
    //
    //End Function

    std::set<U64> foos = {};

    for (U32 i = 0; i < MdHeader.VertexCount; i++)
    {
      foos.emplace(vertices[i].Connection);
    }

    // 0
    // 16145
    // 16649

    if (MdHeader.VertexCount >= 3)
    {
      for (U16 i = 2; i < MdHeader.VertexCount; i++)
      {
        if (vertices[i].Connection > 0)
        {
          //elements.emplace_back(i - 2);
          //elements.emplace_back(i - 1);
          //elements.emplace_back(i - 0);
        }
        else
        {
          elements.emplace_back(i - 1);
          elements.emplace_back(i - 2);
          elements.emplace_back(i - 0);
        }
      }
    }

    for (U16 i = 0; i < MdHeader.VertexCount; i++)
    {
      R32V3 position = (MdHeader.VertexCount == vertices.size()) ? R32V3{ vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z } : R32V3{};
      R32V2 textureMap = (MdHeader.VertexCount == textureMaps.size()) ? R32V2{ 1000.0f / textureMaps[i].x, 10000.0f / textureMaps[i].y } : R32V2{};
      R32V2 textureUv = (MdHeader.VertexCount == textureUvs.size()) ? R32V2{ textureUvs[i].x, textureUvs[i].y } : R32V2{};
      U32 colorWeight = (MdHeader.VertexCount == colorWeights.size()) ? colorWeights[i] : 0;

      ModelDivision.AddVertex(DefaultVertex{ position, textureMap, textureUv, colorWeight });
    }

    for (U16 i = 0; i < elements.size(); i++)
    {
      ModelDivision.AddElement(elements[i]);
    }
  }
}
