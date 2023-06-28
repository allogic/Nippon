#include <Editor/Math.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  R32 Math::Remap(R32 Value, R32 L0, R32 H0, R32 L1, R32 H1)
  {
    return L1 + (Value - L0) * (H1 - L1) / (H0 - L0);
  }

  AABB Math::ComputeBoundingBox(const std::vector<DefaultVertex>& Vertices, const R32V3& Scale)
  {
    R32V3 min = Vertices[0].Position * Scale;
    R32V3 max = Vertices[0].Position * Scale;

    for (const auto& vertex : Vertices)
    {
      R32V3 position = vertex.Position * Scale;

      if (position.x < min.x) min.x = position.x;
      if (position.y < min.y) min.y = position.y;
      if (position.z < min.z) min.z = position.z;

      if (position.x > max.x) max.x = position.x;
      if (position.y > max.y) max.y = position.y;
      if (position.z > max.z) max.z = position.z;
    }

    R32V3 size = glm::abs(min - max);
    R32V3 center = min + size / 2.0F;

    return { min, max, size, center };
  }
}