#pragma once

#include <vector>
#include <filesystem>
#include <algorithm>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class TextureUtils
	{
	public:

		static void WriteJPG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File);
		static void WritePNG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File);
	};
}
