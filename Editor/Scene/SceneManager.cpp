#include <Common/Memory.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Scene/Scene.h>
#include <Editor/Scene/SceneManager.h>
#include <Editor/Scene/LevelScene.h>
#include <Editor/Scene/EntityScene.h>

namespace Nippon
{
	struct SceneByUniqueId
	{
		U64 UniqueId;

		bool operator () (Scene* Scene) const
		{
			return UniqueId == Scene->GetUniqueId();
		};
	};

	static U32 sFreeUniqueArchiveId = 0xFFFFFFFF;

	static bool sIsDirty = false;

	static Scene* sPrevScene = nullptr;
	static Scene* sCurrScene = nullptr;

	static std::vector<Scene*> sScenes = {};

	void SceneManager::Create()
	{

	}

	void SceneManager::Destroy()
	{
		for (auto const& scene : sScenes)
		{
			scene->~Scene();

			Memory::Free(scene);
		}
	}

	Scene* SceneManager::GetCurrentScene()
	{
		return sCurrScene;
	}

	void SceneManager::SetDirty()
	{
		sIsDirty = true;
	}

	void SceneManager::SetCurrentScene(Scene* Scene)
	{
		sCurrScene = Scene;

		if (sCurrScene != sPrevScene)
		{
			Outline::Reset();

			sPrevScene = sCurrScene;
		}
	}

	void SceneManager::FixedUpdate()
	{
		for (auto const& scene : sScenes)
		{
			if (Viewport* viewport = scene->GetViewport())
			{
				if (viewport->IsFocused())
				{
					scene->FixedUpdate();
				}
			}
		}
	}

	void SceneManager::Update()
	{
		if (sIsDirty)
		{
			sIsDirty = false;

			CheckScenesToBeDestroyed();
		}

		for (auto const& scene : sScenes)
		{
			if (Viewport* viewport = scene->GetViewport())
			{
				scene->Update();
			}
		}
	}

	void SceneManager::Render()
	{
		for (auto const& scene : sScenes)
		{
			if (Viewport* viewport = scene->GetViewport())
			{
				viewport->Render();
			}
		}
	}

	void SceneManager::CheckScenesToBeDestroyed()
	{
		for (auto it = sScenes.begin(); it != sScenes.end();)
		{
			Scene* scene = *it;

			if (scene->GetShouldBeDestroyed())
			{
				if (scene == sCurrScene)
				{
					Outline::Reset();
				
					sCurrScene = nullptr;
					sPrevScene = nullptr;
				}

				scene->~Scene();

				Memory::Free(scene);

				it = sScenes.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	Scene* SceneManager::CreateSceneFromFile(fs::path const& FilePath, ArchiveType ArchiveType, bool SkipSceneEmplacement)
	{
		Scene* scene = nullptr;

		ArchiveInfo archiveInfo = {};
		archiveInfo.UniqueId = sFreeUniqueArchiveId;
		archiveInfo.ArchiveType = ArchiveType;
		archiveInfo.FolderId = "";
		archiveInfo.ArchiveId = "";
		archiveInfo.FolderName = "";
		archiveInfo.ArchiveName = "";
		archiveInfo.WindowName = FilePath.string();
		archiveInfo.FilePath = FilePath.string();

		sFreeUniqueArchiveId--;

		auto const& findIt = std::find_if(sScenes.begin(), sScenes.end(), SceneByUniqueId{ archiveInfo.UniqueId });

		if (findIt == sScenes.end())
		{
			switch (archiveInfo.ArchiveType)
			{
				case eArchiveTypeLevel: scene = new (Memory::Alloc(sizeof(LevelScene))) LevelScene{ archiveInfo, true }; break;
				case eArchiveTypeEntity: scene = new (Memory::Alloc(sizeof(EntityScene))) EntityScene{ archiveInfo, true }; break;
			}

			if (!SkipSceneEmplacement)
			{
				sScenes.emplace_back(scene);

				SetCurrentScene(scene);
			}
		}

		return scene;
	}

	Scene* SceneManager::CreateSceneFromDatabase(ArchiveInfo const& ArchiveInfo, bool SkipSceneEmplacement)
	{
		Scene* scene = nullptr;

		auto const& findIt = std::find_if(sScenes.begin(), sScenes.end(), SceneByUniqueId{ ArchiveInfo.UniqueId });

		if (findIt == sScenes.end())
		{
			switch (ArchiveInfo.ArchiveType)
			{
				case eArchiveTypeLevel: scene = new (Memory::Alloc(sizeof(LevelScene))) LevelScene{ ArchiveInfo, false }; break;
				case eArchiveTypeEntity: scene = new (Memory::Alloc(sizeof(EntityScene))) EntityScene{ ArchiveInfo, false }; break;
			}

			if (!SkipSceneEmplacement)
			{
				sScenes.emplace_back(scene);

				SetCurrentScene(scene);
			}
		}

		return scene;
	}

	void SceneManager::DestroyScene(Scene* Scene, bool DestroyImmediately)
	{
		if (DestroyImmediately)
		{
			Scene->~Scene();

			Memory::Free(Scene);
		}
		else
		{
			Scene->SetShouldBeDestroyed();
		}
	}
}