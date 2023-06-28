#pragma once

#include <Common/Types.h>

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
  struct MdTransform
  {
    I16 Unknown1;
    I16 Unknown2;
    U32 Unknown3;
    R32V3 Scale;
    R32V3 Rotation;
    R32V3 Position;
    U8 Padding[20];
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct ScrVertex
  {
    I16 X;
    I16 Y;
    I16 Z;
    U16 Connection;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct ScrTextureMap
  {
    U16 U;
    U16 V;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct ScrUv
  {
    U16 U;
    U16 V;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct ScrColorWeight
  {
    U8 R;
    U8 G;
    U8 B;
    U8 A;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct MdVertex
  {
    R32 X;
    R32 Y;
    R32 Z;
    U16 Connection;
    U16 Unknown1;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct MdTextureMap
  {
    U16 U;
    U16 V;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct MdUv
  {
    U16 U;
    U16 V;
    U16 Unknown1;
    U16 Unknown2;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct MdColorWeight
  {
    U8 R;
    U8 G;
    U8 B;
    U8 A;
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

#pragma pack(push, 1)
  struct ObjEntry
  {
    U8 Id; // lots of values 00-fe 
    U8 Category; // category id - which folder it comes from
    U8 Unknown3; // 00, 10, 20, 30, 40, 50, 60, 70, 80, 90
    U8 Unknown4; // always 00
    U8V3 Scale;
    U8V3 Rotation;
    U16V3 Position;
    U32 Unknown5; // a lot of values, only first byte is used.
    U32 Unknown6; // a lot of values, only first byte is used.
    U8 Unknown7; // 00-0c
    U8 Unknown8; // lots of values
    U8 Unknown9; // bunch, 0*, 4* or 70 (not all values of * though)
    U8 UnknownA; // lots of values
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct ItsEntry
  {
    U8 ContentId;
    U8 Unknown1; // always 0a
    U8 Unknown2; // always 01
    U8 ContainerState;
    U8V3 Scale;
    U8V3 Rotation;
    U8V3 Position;
    U8 ContainerType;
    U8 Unknowns[23];
  };
#pragma pack(pop)
}