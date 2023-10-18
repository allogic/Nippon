#include <Common/Macros.h>

#include <Common/Generated/SceneInfos.h>

#include <Editor/InterfaceManager.h>
#include <Editor/Texture.h>
#include <Editor/TextureLoader.h>

#include <Editor/Interface/EntityBrowser.h>
#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/LevelBrowser.h>
#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/Outline.h>

namespace ark
{
	static std::map<std::string, Texture2D*> sLevelThumbnails = {};
	static std::map<std::string, Texture2D*> sEntityThumbnails = {};

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
		CreateThumbnails();

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

		DestroyThumbnails();
	}

	Texture2D* InterfaceManager::GetLevelThumbnail(const std::string& ThumbnailFileName)
	{
		return sLevelThumbnails[ThumbnailFileName];
	}

	Texture2D* InterfaceManager::GetEntityThumbnail(const std::string& ThumbnailFileName)
	{
		return sEntityThumbnails[ThumbnailFileName];
	}

	void InterfaceManager::CreateThumbnails()
	{
		fs::path file = "Thumbnails";

		for (const auto& groupInfo : SceneInfos::GetLevelGroups())
		{
			for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
			{
				if (fs::exists(file / sceneInfo.ThumbnailFileName))
				{
					sLevelThumbnails[sceneInfo.ThumbnailFileName] = TextureLoader::LoadGeneric(file / sceneInfo.ThumbnailFileName);
				}
				else
				{
					LOG("Missing thumbnail %s\n", sceneInfo.ThumbnailFileName.c_str());
				}
			}
		}

		for (const auto& groupInfo : SceneInfos::GetEntityGroups())
		{
			for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
			{
				if (fs::exists(file / sceneInfo.ThumbnailFileName))
				{
					sEntityThumbnails[sceneInfo.ThumbnailFileName] = TextureLoader::LoadGeneric(file / sceneInfo.ThumbnailFileName);
				}
				else
				{
					LOG("Missing thumbnail %s\n", sceneInfo.ThumbnailFileName.c_str());
				}
			}
		}
	}

	void InterfaceManager::DestroyThumbnails()
	{
		for (const auto& groupInfo : SceneInfos::GetEntityGroups())
		{
			for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
			{
				delete sEntityThumbnails[sceneInfo.ThumbnailFileName];
				sEntityThumbnails[sceneInfo.ThumbnailFileName] = nullptr;
			}
		}

		for (const auto& groupInfo : SceneInfos::GetLevelGroups())
		{
			for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
			{
				delete sLevelThumbnails[sceneInfo.ThumbnailFileName];
				sLevelThumbnails[sceneInfo.ThumbnailFileName] = nullptr;
			}
		}
	}
}
