#pragma once

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  enum VertexType
  {
    eVertexTypeDebug,
  };

  #pragma pack(push, 1)
  struct DebugVertex
  {
    static constexpr VertexType Type{ eVertexTypeDebug };

    R32V3 Position;
    R32V4 Color;
  };
  #pragma pack(pop)
}