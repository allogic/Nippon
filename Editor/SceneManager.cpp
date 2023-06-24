#include <Editor/SceneManager.h>

#include <Editor/Scene.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Outline* gOutline;

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
      gOutline->Reset();

      sActiveScenePrev = sActiveScene;
    }
  }

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
    std::string windowName = "/" + Entry + "/" + SubEntry + " - " + Name;

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
    if (Scene == sActiveScene)
    {
      gOutline->Reset();

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