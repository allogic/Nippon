#pragma once

#include <filesystem>
#include <vector>

#include <Common/Types.h>

namespace ark
{
    namespace fs = std::filesystem;

    class Texture2D;

    class TextureLoader
    {
    public:

        static Texture2D* LoadDirectDrawSurface(U8* Bytes, U64 Size);
        static Texture2D* LoadGeneric(const fs::path& File);
    };
}
