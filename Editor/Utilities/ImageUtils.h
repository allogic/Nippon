#pragma once

#include <filesystem>
#include <vector>

#include <Common/Types.h>

namespace ark
{
    namespace fs = std::filesystem;

    class ImageUtils
    {
    public:

        static U32 ReadDDS(const fs::path& File);
        static U32 ReadDDS(U8* Bytes, U64 Size);

        static U32 ReadJPG(const fs::path& File);
        static U32 ReadJPG(U8* Bytes, U64 Size);

        static U32 ReadPNG(const fs::path& File);
        static U32 ReadPNG(U8* Bytes, U64 Size);

        static void WriteJPG(U32 Id, const fs::path& File);
        //static std::vector<U8> WriteJPG(U32 Id);

        static void WritePNG(U32 Id, const fs::path& File);
        static std::vector<U8> WritePNG(U32 Id);
    };
}
