#include <Editor/Converter/VertexConverter.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<DefaultVertex> VertexConverter::ToVertexBuffer(const std::vector<ScrVertex>& Vertices, const std::vector<ScrTextureMap>& TextureMaps, const std::vector<ScrUv>& TextureUvs, const std::vector<ScrColorWeight>& ColorWeights)
  {
    std::vector<DefaultVertex> vertices = {};

    U16 vertexCount = (U16)Vertices.size();

    vertices.resize(vertexCount);

    for (U16 i = 0; i < vertexCount; i++)
    {
      R32V3 position = (vertexCount == Vertices.size()) ? R32V3{ Vertices[i].Position.x, Vertices[i].Position.y, Vertices[i].Position.z } : R32V3{};
      R32V2 textureMap = (vertexCount == TextureMaps.size()) ? (R32V2{ TextureMaps[i].U, TextureMaps[i].V } / 65535.0F) / 2.0F : R32V2{};
      R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].U, TextureUvs[i].V } / 65535.0F : R32V2{};
      R32V4 colorWeight = (vertexCount == ColorWeights.size()) ? R32V4{ ColorWeights[i].R, ColorWeights[i].G, ColorWeights[i].B, ColorWeights[i].A } / 255.0F : R32V4{};

      vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight };
    }

    return vertices;
  }

  std::vector<DefaultVertex> VertexConverter::ToVertexBuffer(const std::vector<MdVertex>& Vertices, const std::vector<MdTextureMap>& TextureMaps, const std::vector<MdUv>& TextureUvs, const std::vector<MdColorWeight>& ColorWeights)
  {
    std::vector<DefaultVertex> vertices = {};

    U16 vertexCount = (U16)Vertices.size();

    vertices.resize(vertexCount);

    for (U16 i = 0; i < vertexCount; i++)
    {
      R32V3 position = (vertexCount == Vertices.size()) ? Vertices[i].Position : R32V3{};
      R32V2 textureMap = (vertexCount == TextureMaps.size()) ? (R32V2{ TextureMaps[i].U, TextureMaps[i].V } / 65535.0F) / 2.0F : R32V2{};
      R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].U, TextureUvs[i].V } / 65535.0F : R32V2{};
      R32V4 colorWeight = (vertexCount == ColorWeights.size()) ? R32V4{ ColorWeights[i].R, ColorWeights[i].G, ColorWeights[i].B, ColorWeights[i].A } / 255.0F : R32V4{};

      vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight };
    }

    return vertices;
  }
}