#include <Common/ExtensionIterator.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	ExtensionIterator::ExtensionIterator(const fs::path& File, const std::set<std::string>& Extensions)
	{
		if (fs::is_directory(File))
		{
			for (const auto& file : fs::directory_iterator{ File })
			{
				if (Extensions.contains(file.path().extension().string()))
				{
					mFiles.emplace_back(file);
				}
			}
		}
	}
}