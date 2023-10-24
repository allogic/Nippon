#include <Editor/InterfaceManager.h>

#include <Editor/Interface/EntityBrowser.h>
#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/LevelBrowser.h>
#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/Outline.h>

namespace ark
{
	static EntityBrowser* sEntityBrowser = nullptr;
	static Inspector* sInspector = nullptr;
	static LevelBrowser* sLevelBrowser = nullptr;
	static MainMenu* sMainMenu = nullptr;
	static Outline* sOutline = nullptr;

	EntityBrowser* InterfaceManager::GetEntityBrowser() { return sEntityBrowser; }
	Inspector* InterfaceManager::GetInspector() { return sInspector; }
	LevelBrowser* InterfaceManager::GetLevelBrowser() { return sLevelBrowser; }
	MainMenu* InterfaceManager::GetMainMenu() { return sMainMenu; }
	Outline* InterfaceManager::GetOutline() { return sOutline; }

	void InterfaceManager::Create()
	{
		sEntityBrowser = new EntityBrowser;
		sInspector = new Inspector;
		sLevelBrowser = new LevelBrowser;
		sMainMenu = new MainMenu;
		sOutline = new Outline;
	}

	void InterfaceManager::Render()
	{
		sEntityBrowser->Render();
		sInspector->Render();
		sLevelBrowser->Render();
		sMainMenu->Render();
		sOutline->Render();
	}

	void InterfaceManager::Destroy()
	{
		if (sEntityBrowser) delete sEntityBrowser;
		if (sInspector) delete sInspector;
		if (sLevelBrowser) delete sLevelBrowser;
		if (sMainMenu) delete sMainMenu;
		if (sOutline) delete sOutline;

		sEntityBrowser = nullptr;
		sInspector = nullptr;
		sLevelBrowser = nullptr;
		sMainMenu = nullptr;
		sOutline = nullptr;
	}
}
