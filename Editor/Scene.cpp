#include <Editor/Scene.h>

#include <Editor/Actors/Player.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Serializer/ModelSerializer.h>
#include <Editor/Serializer/ObjectSerializer.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Scene::Scene(const std::string& RegionId, const std::string& LevelId)
    : mRegionId{ RegionId }
    , mLevelId{ LevelId }
  {
    DeSerialize();

    mMainActor = CreateActor<Player>("Player", nullptr);

    for (const auto& object : mObjects)
    {
      Actor* actor = CreateActor<Actor>("Object_" + std::to_string(object.GetId()) + "_" + std::to_string(object.GetCategory()), nullptr);

      actor->GetTransform()->SetWorldPosition(object.GetPosition());
      actor->GetTransform()->SetWorldRotation(object.GetRotation());
      actor->GetTransform()->SetWorldScale(R32V3{ 1.0F, 1.0F, 1.0F });
    }

    for (const auto& modelGroup : mModelGroups)
    {
      Actor* groupActor = CreateActor<Actor>(modelGroup.GetName(), nullptr);

      for (const auto& modelEntry : modelGroup)
      {
        Actor* entryActor = CreateActor<Actor>(std::to_string(modelEntry.GetId()), groupActor);
        Transform* entryTransform = entryActor->GetTransform();

        entryTransform->SetWorldPosition(modelEntry.GetPosition());
        entryTransform->SetWorldRotation(modelEntry.GetRotation());
        entryTransform->SetWorldScale(modelEntry.GetScale());

        for (const auto& modelDivision : modelEntry)
        {
          Actor* divisionActor = CreateActor<Actor>("Division", entryActor);
          Renderable* divisionRenderable = divisionActor->AttachComponent<Renderable>();

          divisionRenderable->SetVertexBuffer(modelDivision.GetVertexBuffer());
          divisionRenderable->SetElementBuffer(modelDivision.GetElementBuffer());
        }
      }
    }
  }

  Scene::~Scene()
  {
    Serialize();

    for (auto& actor : mActors)
    {
      delete actor;
      actor = nullptr;
    }

    mActors.clear();
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

  void Scene::Update(R32 TimeDelta)
  {
    DebugRenderer::DebugLine(R32V3{ -10000.0F, 0.0F, 0.0F }, R32V3{ 10000.0F, 0.0F, 0.0F }, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
    DebugRenderer::DebugLine(R32V3{ 0.0F, -10000.0F, 0.0F }, R32V3{ 0.0F, 10000.0F, 0.0F }, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
    DebugRenderer::DebugLine(R32V3{ 0.0F, 0.0F, -10000.0F }, R32V3{ 0.0F, 0.0F, 10000.0F }, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

    for (const auto& actor : mActors)
    {
      actor->Update(TimeDelta);

      Transform* transform = actor->GetTransform();
      Renderable* renderable = actor->GetComponent<Renderable>();

      if (transform && renderable)
      {
        DefaultRenderer::AddRenderTask(RenderTask{ transform, renderable->GetMeshPtr() });
      }

      if (actor != mMainActor)
      {
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        if (actor->GetParent())
        {
          DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition(), R32V4{ 1.0F, 1.0F, 1.0F, 1.0F });
        }

        //DebugRenderer::DebugBox(transform->GetWorldPosition(), transform->GetWorldScale(), R32V4{ 1.0F, 1.0F, 0.0F, 1.0F }, transform->GetQuaternion());
      }
    }
  }

  void Scene::Serialize()
  {

  }

  void Scene::DeSerialize()
  {
    for (const auto& file : fs::directory_iterator{ fs::path{ gConfig["unpackDir"].GetString() } / "levels" / mRegionId / mLevelId })
    {
      if (file.path().extension() == ".TSC") ObjectSerializer{ this, file };
      if (file.path().extension() == ".TRE") ObjectSerializer{ this, file };
      if (file.path().extension() == ".TAT") ObjectSerializer{ this, file };

      if (file.path().extension() == ".SCR") ModelSerializer{ this, file };
    }
  }
}