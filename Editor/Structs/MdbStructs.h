#pragma once

#include <Common/Types.h>

namespace ark
{

#pragma pack(push, 1)

	struct MdbHeader
	{
		U32 MdbId; // always 0x0062646D
		U32 MeshType; // supposedly 20=static, 30=animated, but I've seen 40, 50, 60 (all hex)
		U16 MeshId;
		U16 MeshDivisions;
		U8 Padding[20];
	};

#pragma pack(pop)

}
