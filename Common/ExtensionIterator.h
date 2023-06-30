#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <set>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class ExtensionIterator
	{
	public:

		ExtensionIterator(const fs::path& File, const std::set<std::string>& Extensions);

	private:

		std::vector<fs::path> mFiles = {};
	};
}