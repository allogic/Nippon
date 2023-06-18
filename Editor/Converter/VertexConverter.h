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

    static std::vector<DefaultVertex> ToVertexBuffer(const std::vector<ScrVertex>& Vertices, const std::vector<U16V2>& TextureMaps, const std::vector<U16V2>& TextureUvs, const std::vector<U32>& ColorWeights);
  };
}