#include <Editor/Actor.h>
#include <Editor/Magic.h>

#include <Editor/Converter/VertexConverter.h>

namespace ark
{
	std::vector<DefaultVertex> VertexConverter::ToVertexBuffer(Actor* Actor, const std::vector<ScrVertex>& Vertices, const std::vector<ScrTextureMap>& TextureMaps, const std::vector<ScrUv>& TextureUvs, const std::vector<ScrColorWeight>& ColorWeights)
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

			vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight, Actor->GetId() };
		}

		return vertices;
	}

	std::vector<DefaultVertex> VertexConverter::ToVertexBuffer(Actor* Actor, const std::vector<MdVertex>& Vertices, const std::vector<MdTextureMap>& TextureMaps, const std::vector<MdUv>& TextureUvs, const std::vector<MdColorWeight>& ColorWeights)
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

			vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight, Actor->GetId() };
		}

		return vertices;
	}
}
