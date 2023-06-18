#include <Common/Alignment.h>

#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/ScrSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<std::pair<Model, ScrTransform>> ScrSerializer::FromFile(const fs::path& File)
  {
    std::vector<std::pair<Model, ScrTransform>> models = {};

    U16 index = 0;
    std::string name = "";
    std::string type = "";

    FsUtils::FromArchiveName(File.stem().string(), index, name, type);

    BinaryReader binaryReader{ FsUtils::ReadBinary(File) };

    ScrHeader scrHeader = binaryReader.Read<ScrHeader>();

    models.resize(scrHeader.SubMeshCount);

    std::vector<U32> transformOffsets = binaryReader.Read<U32>(scrHeader.SubMeshCount);

    binaryReader.SeekAbsolute(Align<16>::Up(binaryReader.GetPosition()));

    for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
    {
      Model& model = models[i].first;

      U64 mdbStart = binaryReader.GetPosition();

      model.Header = binaryReader.Read<MdbHeader>();
      model.Name = name;

      if (model.Header.MdbId == 0x0062646D)
      {
        model.Divisions.resize(model.Header.MeshDivisions);

        std::vector<U32> divisionOffsets = binaryReader.Read<U32>(model.Header.MeshDivisions);

        for (U16 j = 0; j < model.Header.MeshDivisions; j++)
        {
          Division& division = model.Divisions[j];

          binaryReader.SeekAbsolute(mdbStart + divisionOffsets[j]);

          U64 mdStart = binaryReader.GetPosition();

          division.Header = binaryReader.Read<MdHeader>();
          
          if (division.Header.VertexOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.VertexOffset);
            binaryReader.Read<ScrVertex>(division.Vertices, division.Header.VertexCount);
          }

          if (division.Header.TextureMapOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.TextureMapOffset);
            binaryReader.Read<U16V2>(division.TextureMaps, division.Header.VertexCount);
          }

          if (division.Header.TextureUvOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.TextureUvOffset);
            binaryReader.Read<U16V2>(division.TextureUvs, division.Header.VertexCount);
          }

          if (division.Header.ColorWeightOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.ColorWeightOffset);
            binaryReader.Read<U32>(division.ColorWeights, division.Header.VertexCount);
          }
        }
      }

      binaryReader.SeekAbsolute(Align<16>::Up(binaryReader.GetPosition()));
    }

    for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
    {
      ScrTransform& transform = models[i].second;

      binaryReader.SeekAbsolute(transformOffsets[i]);

      transform = binaryReader.Read<ScrTransform>();
    }

    return models;
  }

  void ScrSerializer::ToFile(const fs::path& File, const std::vector<std::pair<Model, ScrTransform>>& Objects)
  {
    BinaryWriter binaryWriter = {};

    

    FsUtils::WriteBinary(File, binaryWriter.GetBytes());
  }
}