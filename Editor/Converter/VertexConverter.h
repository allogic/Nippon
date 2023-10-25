#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Vertex.h>

#include <Editor/Structs/ModelStructs.h>

namespace ark
{
	class Actor;

	class VertexConverter
	{
	public:

		static std::vector<DefaultVertex> ToVertexBuffer(Actor* Actor, const std::vector<ScrVertex>& Vertices, const std::vector<TextureMap>& TextureMaps, const std::vector<ScrUv>& TextureUvs, const std::vector<ColorWeight>& ColorWeights);
		static std::vector<DefaultVertex> ToVertexBuffer(Actor* Actor, const std::vector<MdVertex>& Vertices, const std::vector<TextureMap>& TextureMaps, const std::vector<MdUv>& TextureUvs, const std::vector<ColorWeight>& ColorWeights);
	};
}
