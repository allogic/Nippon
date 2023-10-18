#pragma once

#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Common/Generated/SceneInfos.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Thumbnail
	{
	public:

		static void Generate(const SceneInfo& Info);
	};
}
