#pragma once

#include <Common/Types.h>

namespace Nippon
{
#pragma pack(push, 1)

	struct DefaultVertex
	{
		R32V3 Position;
		R32V2 TextureMap;
		R32V2 TextureUv;
		R32V4 ColorWeight;
		U32 EntityId;
	};

	struct DebugVertex
	{
		R32V3 Position;
		R32V4 Color;
	};

#pragma pack(pop)
}