#pragma once

#include <string>
#include <vector>
#include <utility>
#include <filesystem>

#include <Common/Types.h>

#include <Editor/Header.h>
#include <Editor/Model.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class MdSerializer
	{
	public:

		static std::vector<std::pair<MdModel, MdTransform>> FromFile(const fs::path& File);
		static void ToFile(const fs::path& File, const std::vector<std::pair<MdModel, MdTransform>>& Objects);
	};
}