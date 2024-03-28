#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>

#include <Archive/Structs/MeshStructs.h>

#include <OpenGl/VertexTypes.h>

namespace Nippon
{
	class VertexConverter
	{
	public:

		static std::vector<DefaultVertex> ScrToDefault(Entity* Entity, std::vector<ScrVertex> const& Vertices, std::vector<TextureMap> const& TextureMaps, std::vector<ScrUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights);
		static std::vector<DefaultVertex> MdToDefault(Entity* Entity, std::vector<MdVertex> const& Vertices, std::vector<TextureMap> const& TextureMaps, std::vector<MdUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights);
	};
}