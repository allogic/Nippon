#include <Common/ExtensionIterator.h>

#include <Common/Parser/ObjectTableParser.h>
#include <Common/Parser/ModelParser.h>

#include <Editor/Scene.h>

#include <Editor/Actors/Box.h>
#include <Editor/Actors/Player.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

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

    ParseFiles();
  }

  Scene::~Scene()
  {
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

  void Scene::ParseFiles()
  {
    for (const auto file : fs::directory_iterator{ fs::path{ gConfig["unpackDir"].GetString() } / mRegionId / mLevelId })
    {
      if (file.path().extension() == ".TSC")
      {
        ParseObjectTable(file);
      }

      if (file.path().extension() == ".TRE")
      {
        ParseObjectTable(file);
      }

      if (file.path().extension() == ".TAT")
      {
        ParseObjectTable(file);
      }

      if (file.path().extension() == ".SCR")
      {
        ParseModel(file);
      }
    }
  }

  void Scene::ParseObjectTable(const fs::path& File)
  {
    for (const auto& object : ObjectTableParser{ File })
    {
      Box* actor = CreateActor<Box>(std::to_string(object.Id), nullptr);

      actor->GetTransform()->SetWorldPosition(R32V3{ object.Position.x, object.Position.y, object.Position.z });
      actor->GetTransform()->SetWorldRotation(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z } / 255.0F * 360.0F);
      actor->GetTransform()->SetWorldScale(R32V3{ object.Scale.x, object.Scale.y, object.Scale.z });
    }
  }

  void Scene::ParseModel(const fs::path& File)
  {
    for (const auto& model : ModelParser{ File })
    {

    }
  }
}