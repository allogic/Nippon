#pragma once

#include <cassert>
#include <vector>
#include <set>
#include <filesystem>

#include <Common/Types.h>
#include <Common/BinaryReader.h>

#include <Editor/Assets/Model.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  #pragma pack(push, 1)
  struct ScrHeader
  {
    U32 ScrId; // always 0x00726373
    U32 FileType; // 0 for md, 1 for scr?
    U32 SubMeshCount; // for many but not all files, this is the number of submeshes.
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
    U16V3 Scale;
    I16V3 Rotation;
    I16V3 Position;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct ScrVertex
  {
    I16V3 Position;
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

    ModelSerializer(Scene* Scene, const fs::path& File);

  private:

    void ParseScr(U64 ScrStart, ScrHeader& ScrHeader);
    void ParseScrModel(U64 MdbStart, MdbHeader& MdbHeader, ModelGroup& ModelGroup);
    void ParseScrDivision(U64 MdStart, MdHeader& MdHeader, ModelDivision& ModelDivision);

    void ParseMd(U64 ScrStart, ScrHeader& ScrHeader);
    void ParseMdModel(U64 MdbStart, MdbHeader& MdbHeader, ModelGroup& ModelGroup);
    void ParseMdDivision(U64 MdStart, MdHeader& MdHeader, ModelDivision& ModelDivision);

  private:

    Scene* mScene;
    const fs::path mFile;
    BinaryReader mBinaryReader;
  };
}