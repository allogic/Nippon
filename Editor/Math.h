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
    R32V3 Size;
    R32V3 Center;
  };

  class Math
  {
  public:

    static R32 Remap(R32 Value, R32 L0, R32 H0, R32 L1, R32 H1);

    static AABB ComputeBoundingBox(const std::vector<DefaultVertex>& Vertices, const R32V3& Scale);
  };
}