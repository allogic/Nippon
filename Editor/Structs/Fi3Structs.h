#pragma once

#include <Common/Types.h>

namespace ark
{

#pragma pack(push, 1)

    typedef struct
    {
        U8 Unknown0[14];
        U8 Values0[16];
        U8 Values1[16];
        U8 Values2[16];
        U8 Unknown1[50];
    } Fi2Entry;

#pragma pack(pop)

}
