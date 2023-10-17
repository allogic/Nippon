#pragma once

#include <Common/Types.h>

namespace ark
{

#pragma pack(push, 1)

	typedef struct
	{
		U8 Id;
		U8 Unknown1; // always 0a
		U8 Unknown2; // always 01
		U8 ContainerState;
		U8V3 Scale;
		U8V3 Rotation;
		U8V3 Position;
		U8 ContainerType;
		U8 Unknowns[23];
	} ItsEntry;

#pragma pack(pop)

}
