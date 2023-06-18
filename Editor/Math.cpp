#include <Editor/Math.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  AABB Math::ComputeBoundingBox(const std::vector<DefaultVertex>& Vertices)
  {
    R32V3 min = {};
    R32V3 max = {};

    for (const auto& vertex : Vertices)
    {
      if (vertex.Position.x < min.x) min.x = vertex.Position.x;
      if (vertex.Position.y < min.y) min.y = vertex.Position.y;
      if (vertex.Position.z < min.z) min.z = vertex.Position.z;

      if (vertex.Position.x > max.x) max.x = vertex.Position.x;
      if (vertex.Position.y > max.y) max.y = vertex.Position.y;
      if (vertex.Position.z > max.z) max.z = vertex.Position.z;
    }

    return { min, max };
  }
}