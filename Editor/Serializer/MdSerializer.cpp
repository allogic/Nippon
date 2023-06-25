#include <Common/Alignment.h>

#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/MdSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<std::pair<MdModel, MdTransform>> MdSerializer::FromFile(const fs::path& File)
  {
    std::vector<std::pair<MdModel, MdTransform>> models = {};

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
      MdModel& model = models[i].first;

      U64 mdbStart = binaryReader.GetPosition();

      model.Header = binaryReader.Read<MdbHeader>();
      model.Name = name;

      if (model.Header.MdbId == 0x0062646D)
      {
        model.Divisions.resize(model.Header.MeshDivisions);

        std::vector<U32> divisionOffsets = binaryReader.Read<U32>(model.Header.MeshDivisions);

        for (U16 j = 0; j < model.Header.MeshDivisions; j++)
        {
          MdDivision& division = model.Divisions[j];

          binaryReader.SeekAbsolute(mdbStart + divisionOffsets[j]);

          U64 mdStart = binaryReader.GetPosition();

          division.Header = binaryReader.Read<MdHeader>();

          if (division.Header.VertexOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.VertexOffset);
            binaryReader.Read<MdVertex>(division.Vertices, division.Header.VertexCount);
          }

          if (division.Header.TextureMapOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.TextureMapOffset);
            binaryReader.Read<MdTextureMap>(division.TextureMaps, division.Header.VertexCount);
          }

          if (division.Header.TextureUvOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.TextureUvOffset);
            binaryReader.Read<MdUv>(division.TextureUvs, division.Header.VertexCount);
          }

          if (division.Header.ColorWeightOffset != 0)
          {
            binaryReader.SeekAbsolute(mdStart + division.Header.ColorWeightOffset);
            binaryReader.Read<MdColorWeight>(division.ColorWeights, division.Header.VertexCount);
          }
        }
      }

      binaryReader.SeekAbsolute(Align<16>::Up(binaryReader.GetPosition()));
    }

    for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
    {
      MdTransform& transform = models[i].second;

      binaryReader.SeekAbsolute(transformOffsets[i]);

      transform = binaryReader.Read<MdTransform>();
    }

    return models;
  }

  void MdSerializer::ToFile(const fs::path& File, const std::vector<std::pair<MdModel, MdTransform>>& Objects)
  {
    BinaryWriter binaryWriter = {};



    FsUtils::WriteBinary(File, binaryWriter.GetBytes());
  }
}