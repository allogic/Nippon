#include <Common/Macros.h>

#include <Editor/Scene.h>

#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>

#include <Editor/Databases/FileDatabase.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

namespace ark
{
	struct SceneByIdentifier
	{
		U32 Identifier;

		bool operator () (Scene* Scene) const
		{
			return Identifier == Scene->GetFileContainer()->GetIdentifier();
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

	void SceneManager::Update()
	{
		for (const auto& scene : sScenes)
		{
			scene->Update();
		}

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

	Scene* SceneManager::CreateScene(const FileContainer* FileContainer)
	{
		Scene* scene = nullptr;

		const auto findIt = std::find_if(sScenes.begin(), sScenes.end(), SceneByIdentifier{ FileContainer->GetIdentifier() });

		if (findIt == sScenes.end())
		{
			switch (FileContainer->GetType())
			{
				case FileContainer::eFileTypeLevel: scene = sScenes.emplace_back(new LevelScene{ FileContainer }); break;
				case FileContainer::eFileTypeEntity: scene = sScenes.emplace_back(new EntityScene{ FileContainer }); break;
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
