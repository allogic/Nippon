#pragma once

#include <filesystem>
#include <vector>

#include <Common/Types.h>

namespace ark
{
    namespace fs = std::filesystem;

    class TextureLoader
    {
    public:

        static U32 LoadDDS(const fs::path& File);
        static U32 LoadDDS(U8* Bytes, U64 Size);

        static U32 LoadJPG(const fs::path& File);
        static U32 LoadJPG(U8* Bytes, U64 Size);

        static U32 LoadPNG(const fs::path& File);
        static U32 LoadPNG(U8* Bytes, U64 Size);
    };
}
