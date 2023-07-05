#pragma once

#include <string>
#include <vector>
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
	class ScrSerializer
	{
	public:

		static ScrGroup FromFile(const fs::path& File);
		static void ToFile(const fs::path& File, const ScrGroup& Group);
	};
}