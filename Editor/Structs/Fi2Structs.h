#pragma once

#include <Common/Types.h>

namespace ark
{

#pragma pack(push, 1)

	struct Fi2Entry
	{
        U8 Id;
        U8 Category;
        U8 Unknown0;
        U8 Unknown1;
        U32 Unknown2;
        U32 Values0[4];
        U32 Unknown3[22];
        U32 Values1[5];
        U32 Unknown4[8];
        U32 Values2[3];
        U32 Unknown5[9];
        U32 Values3[5];
        U32 Unknown6[8];
        U32 Values4[3];
        U32 Unknown7[9];
        U32 Values5[2];
        U32 Unknown8[10];
        U32 Values6[2];
        U32 Unknown9[66];
	};

#pragma pack(pop)

}
