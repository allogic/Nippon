#pragma once

#include <filesystem>

#include <Common/Common.h>
#include <Common/Types.h>
#include <Common/BinaryReader.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  namespace fs = std::filesystem;

  enum ScrFileType
  {
    eScrFileTypeMd = 0,
    eScrFileTypeScr = 1,
  };

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
    U32 TmOffset; // implicit texture mapping
    U32 CwOffset; // texture colour weight
    U32 UvOffset; // texture uv
    U16 VertexCount; // count
    U16 TextureIndex; // file index of DDS in DDP
    //probably need padding
  };
  #pragma pack(pop)

  class ModelParser
  {
  public:

    ModelParser(const fs::path& File);

  public:

    inline auto begin() { return mModels.begin(); }
    inline const auto begin() const { return mModels.cbegin(); }

    inline auto end() { return mModels.end(); }
    inline const auto end() const { return mModels.cend(); }

  private:

    void ParseScr();
    void ParseMdb();
    void ParseMd();

  private:

    BinaryReader mBinaryReader;

    std::vector<U32> mModels;
    std::vector<ScrTransform> mTransforms;
  };
}