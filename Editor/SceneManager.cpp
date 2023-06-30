#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>

#include <Editor/Scene.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static ark::Scene* sActiveScenePrev = nullptr;
static ark::Scene* sActiveScene = nullptr;

static std::map<std::string, ark::LevelScene*> sLevelScenes = {};
static std::map<std::string, ark::EntityScene*> sEntityScenes = {};

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Scene* SceneManager::GetActiveScene()
  {
    return sActiveScene;
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

  void SceneManager::CreateLevel(const std::string& Entry, const std::string& SubEntry, rj::Value& Value)
  {
    std::string sceneName = "/" + Entry + "/" + SubEntry;
    std::string windowName = "/" + Entry + "/" + SubEntry + " - " + Value["name"].GetString();

    if (sLevelScenes[sceneName])
    {
      delete sLevelScenes[sceneName];
      sLevelScenes[sceneName] = nullptr;
    }

    sLevelScenes[sceneName] = new LevelScene{ Entry, SubEntry, sceneName, windowName };
  }

  void SceneManager::CreateEntity(const std::string& Entry, const std::string& SubEntry, rj::Value& Value)
  {
    std::string sceneName = "/" + Entry + "/" + SubEntry;
    std::string windowName = "/" + Entry + "/" + SubEntry + " - " + Value["name"].GetString();

    if (sEntityScenes[sceneName])
    {
      delete sEntityScenes[sceneName];
      sEntityScenes[sceneName] = nullptr;
    }

    sEntityScenes[sceneName] = new EntityScene{ Entry, SubEntry, sceneName, windowName };
  }

  void SceneManager::Draw()
  {
    for (const auto& [name, scene] : sLevelScenes)
    {
      if (scene)
      {
        Viewport* viewport = scene->GetViewport();

        if (viewport)
        {
          viewport->Draw();
        }
      }
    }

    for (const auto& [name, scene] : sEntityScenes)
    {
      if (scene)
      {
        Viewport* viewport = scene->GetViewport();

        if (viewport)
        {
          viewport->Draw();
        }
      }
    }
  }

  void SceneManager::Destroy(Scene* Scene)
  {
    if (Scene == sActiveScene)
    {
      InterfaceManager::GetOutline()->Reset();

      sActiveScenePrev = nullptr;
      sActiveScene = nullptr;
    }

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