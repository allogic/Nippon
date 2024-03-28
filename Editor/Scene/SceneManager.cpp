#include <Memory.h>

#include <Interface/Viewport.h>
#include <Interface/Outline.h>

#include <Scene/Scene.h>
#include <Scene/SceneManager.h>
#include <Scene/LevelScene.h>
#include <Scene/EntityScene.h>

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

	Scene* SceneManager::CreateScene(ArchiveInfo const& ArchiveInfo, bool SkipSceneEmplacement)
	{
		Scene* scene = nullptr;

		auto const& findIt = std::find_if(sScenes.begin(), sScenes.end(), SceneByUniqueId{ ArchiveInfo.UniqueId });

		if (findIt == sScenes.end())
		{
			switch (ArchiveInfo.ArchiveType)
			{
				case eArchiveTypeLevel: scene = new (Memory::Alloc(sizeof(LevelScene))) LevelScene{ ArchiveInfo }; break;
				case eArchiveTypeEntity: scene = new (Memory::Alloc(sizeof(EntityScene))) EntityScene{ ArchiveInfo }; break;
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