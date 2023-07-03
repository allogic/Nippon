#pragma once

#include <string>
#include <filesystem>
#include <map>

#include <Common/Types.h>

#include <Editor/Interface.h>
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
	class LevelBrowser : public Interface
	{
	public:

		LevelBrowser();
		virtual ~LevelBrowser();

	public:

		virtual void Reset() override;
		virtual void Draw() override;

	private:

		std::map<std::string, std::map<std::string, Texture2D*>> mThumbnails = {};

		std::string mSelectedEntryDir = "";
	};
}