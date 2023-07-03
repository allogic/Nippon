#include <Editor/InterfaceManager.h>

#include <Editor/Interface/EntityBrowser.h>
#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/LevelBrowser.h>
#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/ModelBrowser.h>
#include <Editor/Interface/ObjectBrowser.h>
#include <Editor/Interface/Outline.h>
#include <Editor/Interface/TextureBrowser.h>

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static ark::EntityBrowser* sEntityBrowser = nullptr;
static ark::Inspector* sInspector = nullptr;
static ark::LevelBrowser* sLevelBrowser = nullptr;
static ark::MainMenu* sMainMenu = nullptr;
static ark::ModelBrowser* sModelBrowser = nullptr;
static ark::ObjectBrowser* sObjectBrowser = nullptr;
static ark::Outline* sOutline = nullptr;
static ark::TextureBrowser* sTextureBrowser = nullptr;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	EntityBrowser* InterfaceManager::GetEntityBrowser() { return sEntityBrowser; }
	Inspector* InterfaceManager::GetInspector() { return sInspector; }
	LevelBrowser* InterfaceManager::GetLevelBrowser() { return sLevelBrowser; }
	MainMenu* InterfaceManager::GetMainMenu() { return sMainMenu; }
	ModelBrowser* InterfaceManager::GetModelBrowser() { return sModelBrowser; }
	ObjectBrowser* InterfaceManager::GetObjectBrowser() { return sObjectBrowser; }
	Outline* InterfaceManager::GetOutline() { return sOutline; }
	TextureBrowser* InterfaceManager::GetTextureBrowser() { return sTextureBrowser; }

	void InterfaceManager::Create()
	{
		sEntityBrowser = new EntityBrowser;
		sInspector = new Inspector;
		sLevelBrowser = new LevelBrowser;
		sMainMenu = new MainMenu;
		sModelBrowser = new ModelBrowser;
		sObjectBrowser = new ObjectBrowser;
		sOutline = new Outline;
		sTextureBrowser = new TextureBrowser;
	}

	void InterfaceManager::Draw()
	{
		sEntityBrowser->Draw();
		sInspector->Draw();
		sLevelBrowser->Draw();
		sMainMenu->Draw();
		sModelBrowser->Draw();
		sObjectBrowser->Draw();
		sOutline->Draw();
		sTextureBrowser->Draw();
	}

	void InterfaceManager::Destroy()
	{
		if (sEntityBrowser) delete sEntityBrowser;
		if (sInspector) delete sInspector;
		if (sLevelBrowser) delete sLevelBrowser;
		if (sMainMenu) delete sMainMenu;
		if (sModelBrowser) delete sModelBrowser;
		if (sObjectBrowser) delete sObjectBrowser;
		if (sOutline) delete sOutline;
		if (sTextureBrowser) delete sTextureBrowser;

		sEntityBrowser = nullptr;
		sInspector = nullptr;
		sLevelBrowser = nullptr;
		sMainMenu = nullptr;
		sModelBrowser = nullptr;
		sObjectBrowser = nullptr;
		sOutline = nullptr;
		sTextureBrowser = nullptr;
	}
}