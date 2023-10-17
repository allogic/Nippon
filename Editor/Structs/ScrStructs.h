#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/MdStructs.h>
#include <Editor/Structs/MdbStructs.h>

namespace ark
{

#pragma pack(push, 1)

	typedef struct
	{
		U32 ScrId; // always 0x00726373
		U32 FileType; // 0 for md, 1 for scr?
		U32 SubMeshCount; // for many but not all files, this is the number of submeshes.
		U32 Padding; // always zero
	} ScrHeader;

	typedef struct
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
	} ScrTransform;

	typedef struct
	{
		I16 X;
		I16 Y;
		I16 Z;
		U16 Connection;
	} ScrVertex;

	typedef struct
	{
		U16 U;
		U16 V;
	} ScrTextureMap;

	typedef struct
	{
		U16 U;
		U16 V;
	} ScrUv;

	typedef struct
	{
		U8 R;
		U8 G;
		U8 B;
		U8 A;
	} ScrColorWeight;

#pragma pack(pop)

	typedef struct
	{
		U32 Index;
		MdHeader Header;
		std::vector<ScrVertex> Vertices;
		std::vector<ScrTextureMap> TextureMaps;
		std::vector<ScrUv> TextureUvs;
		std::vector<ScrColorWeight> ColorWeights;
	} ScrDivision;

	typedef struct
	{
		MdbHeader Header;
		std::vector<ScrDivision> Divisions;
	} ScrEntry;

	typedef struct
	{
		U32 Index;
		ScrEntry Entry;
		ScrTransform Transform;
	} ScrModel;

	typedef struct
	{
		std::string Name;
		std::vector<ScrModel> Models;
	} ScrGroup;
}
