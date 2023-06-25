#pragma once

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  enum VertexType
  {
    eVertexTypeDefault,
    eVertexTypeDebug,
  };

#pragma pack(push, 1)
  struct DefaultVertex
  {
    static constexpr VertexType Type = eVertexTypeDefault;

    R32V3 Position;
    R32V2 TextureMap;
    R32V2 TextureUv;
    R32V4 ColorWeight;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct DebugVertex
  {
    static constexpr VertexType Type = eVertexTypeDebug;

    R32V3 Position;
    R32V4 Color;
  };
#pragma pack(pop)
}