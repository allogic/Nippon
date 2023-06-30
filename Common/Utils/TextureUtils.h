#pragma once

#include <vector>
#include <filesystem>
#include <algorithm>

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class TextureUtils
	{
	public:

		static Texture2D* LoadDirectDrawSurface(const fs::path& File);
		static Texture2D* LoadGeneric(const fs::path& File);

		static void WriteJPG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File);
		static void WritePNG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File);
	};
}