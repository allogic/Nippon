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

    static AABB ComputeBoundingBox(const std::vector<DefaultVertex>& Vertices, const R32V3& Scale);
  };
}