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

		static Texture2D* LoadDDS(const fs::path& File);
		static Texture2D* LoadDDS(const std::vector<U8>& Bytes);
	};
}