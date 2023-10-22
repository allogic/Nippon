#pragma once

#include <string>
#include <filesystem>
#include <map>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class EntityBrowser;
	class Inspector;
	class LevelBrowser;
	class MainMenu;
	class Outline;

	class InterfaceManager
	{
	public:

		static EntityBrowser* GetEntityBrowser();
		static Inspector* GetInspector();
		static LevelBrowser* GetLevelBrowser();
		static MainMenu* GetMainMenu();
		static Outline* GetOutline();

	public:

		static void Create();
		static void Destroy();

	public:

		static void Render();

	public:

		static U32 GetLevelThumbnail(const std::string& ThumbnailFileName);
		static U32 GetEntityThumbnail(const std::string& ThumbnailFileName);

	private:

		static void CreateThumbnails();
		static void DestroyThumbnails();
	};
}
