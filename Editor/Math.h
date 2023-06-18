#pragma once

#include <vector>
#include <limits>

#include <Common/Types.h>

#include <Editor/Vertex.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  struct AABB
  {
    R32V3 Min;
    R32V3 Max;
  };

  class Math
  {
  public:

    static AABB ComputeBoundingBox(const std::vector<DefaultVertex>& Vertices);
  };
}