#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#define SCR_HEADER_ID 0x00726373
#define MDB_HEADER_ID 0x0062646D

namespace Nippon
{
#pragma pack(push, 1)

	struct ScrHeader
	{
		U32 ScrId; // always 0x00726373
		U32 FileType; // 0 for md, 1 for scr?
		U32 SubMeshCount; // for many but not all files, this is the number of submeshes.
		U32 Padding; // always zero
	};

	struct MdbHeader
	{
		U32 MdbId; // always 0x0062646D
		U32 MeshType; // supposedly 20=static, 30=animated, but I've seen 40, 50, 60 (all hex)
		U16 MeshId;
		U16 MeshDivisions;
		U8 Padding[20];
	};

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

	struct ScrVertex
	{
		I16 X;
		I16 Y;
		I16 Z;
		U16 Connection;
	};

	struct MdVertex
	{
		R32 X;
		R32 Y;
		R32 Z;
		U16 Connection;
		U16 Unknown1;
	};

	struct TextureMap
	{
		U16 U;
		U16 V;
	};

	struct ScrUv
	{
		U16 U;
		U16 V;
	};

	struct MdUv
	{
		U16 U;
		U16 V;
		U16 Unknown1;
		U16 Unknown2;
	};

	struct ColorWeight
	{
		U8 R;
		U8 G;
		U8 B;
		U8 A;
	};

#pragma pack(pop)

	template<typename Vertex, typename Uv>
	struct GenericSubMesh
	{
		using VertexType = Vertex;
		using UvType = Uv;

		U32 Index;
		MdHeader Header;
		std::vector<Vertex> Vertices;
		std::vector<TextureMap> TextureMaps;
		std::vector<Uv> TextureUvs;
		std::vector<ColorWeight> ColorWeights;
	};

	using ScrSubMesh = GenericSubMesh<ScrVertex, ScrUv>;
	using MdSubMesh = GenericSubMesh<MdVertex, MdUv>;

	template<typename SubMesh, typename Transform>
	struct GenericMesh
	{
		using SubMeshType = SubMesh;
		using TransformType = Transform;

		U32 Index;
		MdbHeader Header;
		std::vector<SubMesh> SubMeshes;
		Transform Transform;
	};

	using ScrMesh = GenericMesh<ScrSubMesh, ScrTransform>;
	using MdMesh = GenericMesh<MdSubMesh, MdTransform>;
}