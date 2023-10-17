#pragma once

#include <Common/Types.h>

namespace ark
{
	typedef enum
	{
		eVertexTypeDefault,
		eVertexTypeDebug,
	} VertexType;

#pragma pack(push, 1)

	struct DefaultVertex
	{
		static constexpr VertexType Type = eVertexTypeDefault;

		R32V3 Position;
		R32V2 TextureMap;
		R32V2 TextureUv;
		R32V4 ColorWeight;
		U32 ActorId;
	};

	struct DebugVertex
	{
		static constexpr VertexType Type = eVertexTypeDebug;

		R32V3 Position;
		R32V4 Color;
	};

#pragma pack(pop)

}
