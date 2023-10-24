#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/MdStructs.h>
#include <Editor/Structs/MdbStructs.h>

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

	struct ScrVertex
	{
		I16 X;
		I16 Y;
		I16 Z;
		U16 Connection;
	};

	struct ScrTextureMap
	{
		U16 U;
		U16 V;
	};

	struct ScrUv
	{
		U16 U;
		U16 V;
	};

	struct ScrColorWeight
	{
		U8 R;
		U8 G;
		U8 B;
		U8 A;
	};

#pragma pack(pop)

	struct ScrDivision
	{
		U32 Index;
		MdHeader Header;
		std::vector<ScrVertex> Vertices;
		std::vector<ScrTextureMap> TextureMaps;
		std::vector<ScrUv> TextureUvs;
		std::vector<ScrColorWeight> ColorWeights;
	};

	struct ScrEntry
	{
		MdbHeader Header;
		std::vector<ScrDivision> Divisions;
	};

	struct ScrModel
	{
		U32 Index;
		ScrEntry Entry;
		ScrTransform Transform;
	};

	struct ScrGroup
	{
		std::string Name;
		std::vector<ScrModel> Models;
	};
}
