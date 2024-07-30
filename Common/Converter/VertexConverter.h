#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>
#include <Common/VertexTypes.h>

#include <Common/Structs/ModelStructs.h>

namespace Nippon
{
	class VertexConverter
	{
	public:

		static std::vector<DefaultVertex> ScrToDefault(U32 EntityId, std::vector<ScrVertex> const& Vertices, std::vector<TextureMap> const& TextureMaps, std::vector<ScrUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights);
		static std::vector<DefaultVertex> MdToDefault(U32 EntityId, std::vector<MdVertex> const& Vertices, std::vector<TextureMap> const& TextureMaps, std::vector<MdUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights);
	};
}