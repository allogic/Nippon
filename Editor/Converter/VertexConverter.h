#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Vertex.h>
#include <Editor/Header.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class VertexConverter
  {
  public:

    static std::vector<DefaultVertex> ToVertexBuffer(const std::vector<ScrVertex>& Vertices, const std::vector<ScrTextureMap>& TextureMaps, const std::vector<ScrUv>& TextureUvs, const std::vector<ScrColorWeight>& ColorWeights);
    static std::vector<DefaultVertex> ToVertexBuffer(const std::vector<MdVertex>& Vertices, const std::vector<MdTextureMap>& TextureMaps, const std::vector<MdUv>& TextureUvs, const std::vector<MdColorWeight>& ColorWeights);
  };
}