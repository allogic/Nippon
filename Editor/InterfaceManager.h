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
	class Texture2D;

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

		static Texture2D* GetLevelThumbnail(const std::string& ThumbnailFileName);
		static Texture2D* GetEntityThumbnail(const std::string& ThumbnailFileName);

	private:

		static void CreateThumbnails();
		static void DestroyThumbnails();
	};
}
