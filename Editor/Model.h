#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Header.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	struct ScrDivision
	{
		U32 Index;
		MdHeader Header;
		std::vector<ScrVertex> Vertices;
		std::vector<ScrTextureMap> TextureMaps;
		std::vector<ScrUv> TextureUvs;
		std::vector<ScrColorWeight> ColorWeights;
	};

	struct MdDivision
	{
		U32 Index;
		MdHeader Header;
		std::vector<MdVertex> Vertices;
		std::vector<MdTextureMap> TextureMaps;
		std::vector<MdUv> TextureUvs;
		std::vector<MdColorWeight> ColorWeights;
	};

	struct ScrEntry
	{
		MdbHeader Header;
		std::vector<ScrDivision> Divisions;
	};

	struct MdEntry
	{
		MdbHeader Header;
		std::vector<MdDivision> Divisions;
	};

	struct ScrModel
	{
		U32 Index;
		ScrEntry Entry;
		ScrTransform Transform;
	};

	struct MdModel
	{
		U32 Index;
		MdEntry Entry;
		MdTransform Transform;
	};

	struct ScrGroup
	{
		std::string Name;
		std::vector<ScrModel> Models;
	};

	struct MdGroup
	{
		std::string Name;
		std::vector<MdModel> Models;
	};
}