#include <Editor/InterfaceManager.h>

#include <Editor/Interface/EntityBrowser.h>
#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/LevelBrowser.h>
#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/Outline.h>

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static ark::EntityBrowser* sEntityBrowser = nullptr;
static ark::Inspector* sInspector = nullptr;
static ark::LevelBrowser* sLevelBrowser = nullptr;
static ark::MainMenu* sMainMenu = nullptr;
static ark::Outline* sOutline = nullptr;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
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

	void InterfaceManager::Draw()
	{
		sEntityBrowser->Draw();
		sInspector->Draw();
		sLevelBrowser->Draw();
		sMainMenu->Draw();
		sOutline->Draw();
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