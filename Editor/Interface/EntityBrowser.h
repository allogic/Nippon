#pragma once

#include <string>
#include <filesystem>
#include <map>

#include <Common/Types.h>

#include <Editor/Interface.h>
#include <Editor/Forward.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;
namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class EntityBrowser : public Interface
	{
	public:

		EntityBrowser();
		virtual ~EntityBrowser();

	public:

		virtual void Reset() override;
		virtual void Draw() override;

	private:

		std::map<std::string, std::map<std::string, Texture2D*>> mThumbnails = {};

		std::string mSelectedEntryDir = "";
	};
}