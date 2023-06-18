#include <Editor/Converter/VertexConverter.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<DefaultVertex> VertexConverter::ToVertexBuffer(const std::vector<ScrVertex>& Vertices, const std::vector<U16V2>& TextureMaps, const std::vector<U16V2>& TextureUvs, const std::vector<U32>& ColorWeights)
  {
    std::vector<DefaultVertex> vertices = {};

    U16 vertexCount = (U16)Vertices.size();

    vertices.resize(vertexCount);

    for (U16 i = 0; i < vertexCount; i++)
    {
      R32V3 position = (vertexCount == Vertices.size()) ? R32V3{ Vertices[i].Position.x, Vertices[i].Position.y, Vertices[i].Position.z } : R32V3{};
      R32V2 textureMap = (vertexCount == TextureMaps.size()) ? (R32V2{ TextureMaps[i].x, TextureMaps[i].y } / 65535.0F) / 2.0F : R32V2{};
      R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].x, TextureUvs[i].y } : R32V2{};
      U32 colorWeight = (vertexCount == ColorWeights.size()) ? ColorWeights[i] : 0;

      vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight };
    }

    return vertices;
  }
}