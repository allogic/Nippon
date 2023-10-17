#include <Common/Macros.h>

#include <Editor/Scene.h>

#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

namespace ark
{
	struct SceneByInfo
	{
		SceneType Type;
		std::string GroupKey;
		std::string SceneKey;

		bool operator () (Scene* Scene) const
		{
			return Type == Scene->GetSceneType() && GroupKey == Scene->GetGroupKey() && SceneKey == Scene->GetSceneKey();
		};
	};

	static Scene* sActiveScenePrev = nullptr;
	static Scene* sActiveScene = nullptr;

	static std::vector<Scene*> sScenes = {};

	static bool sIsDirty = false;

	void SceneManager::Create()
	{
		
	}

	void SceneManager::Destroy()
	{
		for (auto& scene : sScenes)
		{
			delete scene;
			scene = nullptr;
		}
	}

	Scene* SceneManager::GetActiveScene()
	{
		return sActiveScene;
	}

	void SceneManager::SetDirty(bool Value)
	{
		sIsDirty = Value;
	}

	void SceneManager::SetActiveScene(Scene* Scene)
	{
		sActiveScene = Scene;

		if (sActiveScene != sActiveScenePrev)
		{
			InterfaceManager::GetOutline()->Reset();

			sActiveScenePrev = sActiveScene;
		}
	}

	void SceneManager::PreRender()
	{
		// TODO
	}

	void SceneManager::Render()
	{
		for (const auto& scene : sScenes)
		{
			if (Viewport* viewport = scene->GetViewport())
			{
				viewport->Render();
			}
		}
	}

	void SceneManager::PostRender()
	{
		// TODO
	}

	void SceneManager::PreUpdate()
	{
		// TODO
	}

	void SceneManager::Update()
	{
		// TODO
	}

	void SceneManager::PostUpdate()
	{
		// TODO

		if (sIsDirty)
		{
			sIsDirty = false;

			for (auto it = sScenes.begin(); it != sScenes.end();)
			{
				Scene* scene = *it;

				if (scene->GetShouldBeDestroyed())
				{
					if (scene == sActiveScene)
					{
						InterfaceManager::GetOutline()->Reset();

						sActiveScene = nullptr;
						sActiveScenePrev = nullptr;
					}

					delete scene;
					scene = nullptr;

					it = sScenes.erase(it);
				}
				else
				{
					it++;
				}
			}
		}
	}

	Scene* SceneManager::CreateScene(const SceneInfo& Info)
	{
		Scene* scene = nullptr;

		const auto findIt = std::find_if(sScenes.begin(), sScenes.end(), SceneByInfo{ Info.Type, Info.GroupKey, Info.SceneKey });

		if (findIt == sScenes.end())
		{
			switch (Info.Type)
			{
				case eSceneTypeLevel: scene = sScenes.emplace_back(new LevelScene{ Info }); break;
				case eSceneTypeEntity: scene = sScenes.emplace_back(new EntityScene{ Info }); break;
			}

			SetActiveScene(scene);
		}
		else
		{
			scene = *findIt;
		}

		if (Viewport* viewport = scene->GetViewport())
		{
			viewport->SetFocused();
		}

		return scene;
	}

	void SceneManager::DestroyScene(Scene* Scene)
	{
		if (Scene == sActiveScene)
		{
			InterfaceManager::GetOutline()->Reset();

			sActiveScene = nullptr;
			sActiveScenePrev = nullptr;
		}

		delete Scene;

		const auto findIt = std::find(sScenes.begin(), sScenes.end(), Scene);

		if (findIt == sScenes.end())
		{

		}
		else
		{
			sScenes.erase(findIt);
		}
	}
}
