#include <Common/ExtensionIterator.h>

#include <Editor/Scene.h>

#include <Editor/Actors/Box.h>
#include <Editor/Actors/Player.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Editor/Serializer/ModelSerializer.h>
#include <Editor/Serializer/ObjectSerializer.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rapidjson::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Scene::Scene(const std::string& RegionId, const std::string& LevelId)
    : mRegionId{ RegionId }
    , mLevelId{ LevelId }
  {
    SetMainActor(CreateActor<Player>("Player", nullptr));

    DeSerialize();
  }

  Scene::~Scene()
  {
    Serialize();

    for (auto& actor : mActors)
    {
      delete actor;
      actor = nullptr;
    }
  }

  void Scene::DestroyActor(Actor* Actor)
  {
    auto actorIt = std::find(mActors.begin(), mActors.end(), Actor);

    if (actorIt != mActors.end())
    {
      delete *actorIt;
      *actorIt = nullptr;

      mActors.erase(actorIt);
    }
  }

  Actor* Scene::GetMainActor()
  {
    return mMainActor;
  }

  Camera* Scene::GetMainCamera()
  {
    if (mMainActor)
    {
      return mMainActor->GetComponent<Camera>();
    }

    return nullptr;
  }

  void Scene::Serialize()
  {

  }

  void Scene::DeSerialize()
  {
    for (const auto file : fs::directory_iterator{ fs::path{ gConfig["unpackDir"].GetString() } / mRegionId / mLevelId })
    {
      if (file.path().extension() == ".TSC")
      {
        ObjectSerializer{ this, file };
      }

      if (file.path().extension() == ".TRE")
      {
        ObjectSerializer{ this, file };
      }

      if (file.path().extension() == ".TAT")
      {
        ObjectSerializer{ this, file };
      }

      if (file.path().extension() == ".SCR")
      {
        ModelSerializer{ this, file };
      }
    }
  }
}