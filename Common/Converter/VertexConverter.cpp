#include <Common/Magic.h>

#include <Common/Converter/VertexConverter.h>

namespace Nippon
{
	std::vector<DefaultVertex> VertexConverter::ScrToDefault(U32 EntityId, std::vector<ScrVertex> const& Vertices, std::vector<TextureMap> const& TextureMaps, std::vector<ScrUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights)
	{
		std::vector<DefaultVertex> vertices = {};

		U16 vertexCount = (U16)Vertices.size();

		vertices.resize(vertexCount);

		for (U16 i = 0; i < vertexCount; i++)
		{
			R32 U = (R32)TextureMaps[i].U;
			R32 V = (R32)TextureMaps[i].V;

			if (U < 32767.5F) U += 65535.0F;
			if (V < 32767.5F) V += 65535.0F;

			U /= 65535.0F;
			V /= 65535.0F;

			U *= MAGIC_UV_COEFFICIENT;
			V *= MAGIC_UV_COEFFICIENT;

			R32V3 position = (vertexCount == Vertices.size()) ? R32V3{ Vertices[i].X, Vertices[i].Y, Vertices[i].Z } : R32V3{};
			R32V2 textureMap = (vertexCount == TextureMaps.size()) ? R32V2{ U, V } : R32V2{};
			R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].U, TextureUvs[i].V } : R32V2{};
			R32V4 colorWeight = (vertexCount == ColorWeights.size()) ? R32V4{ ColorWeights[i].R, ColorWeights[i].G, ColorWeights[i].B, ColorWeights[i].A } / 255.0F : R32V4{};

			vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight, EntityId };
		}

		return vertices;
	}

	std::vector<DefaultVertex> VertexConverter::MdToDefault(U32 EntityId, std::vector<MdVertex> const& Vertices, std::vector<TextureMap> const& TextureMaps, std::vector<MdUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights)
	{
		std::vector<DefaultVertex> vertices = {};

		U16 vertexCount = (U16)Vertices.size();

		vertices.resize(vertexCount);

		for (U16 i = 0; i < vertexCount; i++)
		{
			R32 U = (R32)TextureMaps[i].U;
			R32 V = (R32)TextureMaps[i].V;

			if (U < 32767.5F) U += 65535.0F;
			if (V < 32767.5F) V += 65535.0F;

			U /= 65535.0F;
			V /= 65535.0F;

			U *= MAGIC_UV_COEFFICIENT;
			V *= MAGIC_UV_COEFFICIENT;

			R32V3 position = (vertexCount == Vertices.size()) ? R32V3{ Vertices[i].X, Vertices[i].Y, Vertices[i].Z } : R32V3{};
			R32V2 textureMap = (vertexCount == TextureMaps.size()) ? R32V2{ U, V } : R32V2{};
			R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].U, TextureUvs[i].V } : R32V2{};
			R32V4 colorWeight = (vertexCount == ColorWeights.size()) ? R32V4{ ColorWeights[i].R, ColorWeights[i].G, ColorWeights[i].B, ColorWeights[i].A } / 255.0F : R32V4{};

			vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight, EntityId };
		}

		return vertices;
	}
}