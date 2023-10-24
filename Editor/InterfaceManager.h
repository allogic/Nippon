#pragma once

#include <Common/Types.h>

namespace ark
{
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
	};
}
