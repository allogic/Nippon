#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/MdbStructs.h>

namespace ark
{

#pragma pack(push, 1)

	typedef struct
	{
		U32 VertexOffset; // offset from this spot to vertex info
		U32 Unknown1; // 0?
		U32 TextureMapOffset; // implicit texture mapping
		U32 ColorWeightOffset; // texture colour weight
		U32 TextureUvOffset; // texture uv
		U16 VertexCount; // count
		U16 TextureIndex; // file index of DDS in DDP
	} MdHeader;

	typedef struct
	{
		I16 Unknown1;
		I16 Unknown2;
		U32 Unknown3;
		R32V3 Scale;
		R32V3 Rotation;
		R32V3 Position;
		U8 Padding[20];
	} MdTransform;

	typedef struct
	{
		R32 X;
		R32 Y;
		R32 Z;
		U16 Connection;
		U16 Unknown1;
	} MdVertex;

	typedef struct
	{
		U16 U;
		U16 V;
	} MdTextureMap;

	typedef struct
	{
		U16 U;
		U16 V;
		U16 Unknown1;
		U16 Unknown2;
	} MdUv;

	typedef struct
	{
		U8 R;
		U8 G;
		U8 B;
		U8 A;
	} MdColorWeight;

#pragma pack(pop)

	typedef struct
	{
		U32 Index;
		MdHeader Header;
		std::vector<MdVertex> Vertices;
		std::vector<MdTextureMap> TextureMaps;
		std::vector<MdUv> TextureUvs;
		std::vector<MdColorWeight> ColorWeights;
	} MdDivision;

	typedef struct
	{
		MdbHeader Header;
		std::vector<MdDivision> Divisions;
	} MdEntry;

	typedef struct
	{
		U32 Index;
		MdEntry Entry;
		MdTransform Transform;
	} MdModel;

	typedef struct
	{
		std::string Name;
		std::vector<MdModel> Models;
	} MdGroup;
}
