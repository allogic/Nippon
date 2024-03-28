#pragma once

#include <Types.h>

namespace Nippon
{
#pragma pack(push, 1)

    struct Fi2Entry
    {
        U8 Unknown0[14];
        U8 Values0[16];
        U8 Values1[16];
        U8 Values2[16];
        U8 Unknown1[50];
    };

#pragma pack(pop)
}