#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
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
		static void Draw();
		static void Destroy();
	};
}