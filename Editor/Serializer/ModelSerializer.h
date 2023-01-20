#pragma once

#include <vector>
#include <utility>
#include <filesystem>

#include <Common/Common.h>
#include <Common/Types.h>
#include <Common/BinaryReader.h>

#include <Editor/Assets/Model.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  namespace fs = std::filesystem;

  #pragma pack(push, 1)
  struct ScrHeader
  {
    U32 ScrId; // always 0x00726373
    U32 FileType; // 0 for md, 1 for scr?
    U32 EndEntryCount; // for many but not all files, this is the number of submeshes.
    U32 Padding; // always zero
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct ScrTransform
  {
    I16 Unknown1;
    I16 Unknown2;
    U32 SubmeshIndex;
    U16 Unknown3;
    U16 Unknown4;
    U16 Unknown5;
    U16 Padding1[3]; // always 00 00;
    U16 Unknown6;
    U16 Unknown7;
    U16 Padding2[5]; // always 00 00;
    PackedTuple<U16> Scale;
    PackedTuple<I16> Rotation;
    PackedTuple<I16> Position;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct ScrVertex
  {
    PackedTuple<I16> Position;
    U16 Connection;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct MdbHeader
  {
    U32 MdbId; // always 0x0062646D
    U32 MeshType; // supposedly 20=static, 30=animated, but I've seen 40, 50, 60 (all hex)
    U16 MeshId;
    U16 MeshDivisions;
    U8 Padding[20];
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct MdHeader
  {
    U32 VertexOffset; // offset from this spot to vertex info
    U32 Unknown1; // 0?
    U32 TextureMapOffset; // implicit texture mapping
    U32 ColorWeightOffset; // texture colour weight
    U32 TextureUvOffset; // texture uv
    U16 VertexCount; // count
    U16 TextureIndex; // file index of DDS in DDP
  };
  #pragma pack(pop)

  class ModelSerializer
  {
  public:

    ModelSerializer(const fs::path& File);

  private:

    void ParseScr();
    void ParseModel(Model& Model);
    std::pair<U16, U16> ParseSubModel(std::vector<ScrVertex>& Vertices, std::vector<PackedPair<U16>>& TextureMaps, std::vector<PackedPair<U16>>& TextureUvs, std::vector<U32>& ColorWeights, std::vector<U32>& Elements);

  private:

    const fs::path mFile;
    BinaryReader mBinaryReader;
  };
}