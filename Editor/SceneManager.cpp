#include <Editor/SceneManager.h>

#include <Editor/Scene.h>

#include <Editor/Interface/Viewport.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static std::map<std::string, ark::LevelScene*> sLevelScenes = {};
static std::map<std::string, ark::EntityScene*> sEntityScenes = {};

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void SceneManager::CreateLevel(const std::string& Entry, const std::string& SubEntry, const std::string& Name)
  {
    std::string sceneName = "/" + Entry + "/" + SubEntry;
    std::string windowName = "/" + Entry + "/" + SubEntry + " - " + Name;

    if (sLevelScenes[sceneName])
    {
      delete sLevelScenes[sceneName];
      sLevelScenes[sceneName] = nullptr;
    }

    sLevelScenes[sceneName] = new LevelScene{ Entry, SubEntry, sceneName, windowName };
  }

  void SceneManager::CreateEntity(const std::string& Entry, const std::string& SubEntry, const std::string& Name)
  {
    std::string sceneName = "/" + Entry + "/" + SubEntry;
    std::string windowName = "Entity";

    if (sEntityScenes[sceneName])
    {
      delete sEntityScenes[sceneName];
      sEntityScenes[sceneName] = nullptr;
    }

    sEntityScenes[sceneName] = new EntityScene{ Entry, SubEntry, sceneName, windowName };
  }

  void SceneManager::DrawAllViewports()
  {
    for (const auto& [name, scene] : sLevelScenes)
    {
      if (scene)
      {
        scene->GetViewport()->Draw();
      }
    }

    for (const auto& [name, scene] : sEntityScenes)
    {
      if (scene)
      {
        scene->GetViewport()->Draw();
      }
    }
  }

  void SceneManager::Destroy(Scene* Scene)
  {
    std::string sceneName = Scene->GetSceneName();

    switch (Scene->GetSceneType())
    {
      case eSceneTypeLevel:
      {
        if (sLevelScenes[sceneName])
        {
          delete sLevelScenes[sceneName];
          sLevelScenes[sceneName] = nullptr;
        }

        break;
      }
      case eSceneTypeEntity:
      {
        if (sEntityScenes[sceneName])
        {
          delete sEntityScenes[sceneName];
          sEntityScenes[sceneName] = nullptr;
        }

        break;
      }
    }
  }
}