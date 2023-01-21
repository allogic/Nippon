#include <Common/ExtensionIterator.h>

#include <Editor/AssetDatabase.h>
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

extern rapidjson::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void Scene::Create(const std::string& RegionId, const std::string& LevelId)
  {
    sRegionId = RegionId;
    sLevelId = LevelId;
    sMainActor = CreateActor<Player>("Player", nullptr);

    DeSerialize();

    //for (const auto& object : AssetDatabase::GetObjects())
    //{
    //  Actor* actor = CreateActor<Actor>("Object", nullptr);
    //  actor->GetTransform()->SetWorldPosition(object.GetPosition());
    //  actor->GetTransform()->SetWorldRotation(object.GetRotation());
    //  actor->GetTransform()->SetWorldScale(object.GetScale());
    //}

    for (const auto& model : AssetDatabase::GetModels())
    {
      Actor* actor = CreateActor<Actor>("Model", nullptr);

      Transform* transform = actor->GetTransform();
      transform->SetWorldPosition(model.GetTransform().Position);
      transform->SetWorldRotation(model.GetTransform().Rotation);
      transform->SetWorldScale(R32V3{ 1.0F, 1.0F, 1.0F });

      Renderable* renderable = actor->AttachComponent<Renderable>();
      renderable->SetVertexBuffer(model.GetVertexBuffer());
      renderable->SetElementBuffer(model.GetElementBuffer());
    }
  }

  void Scene::Destroy()
  {
    AssetDatabase::ClearModels();
    AssetDatabase::ClearObjects();

    Serialize();

    for (auto& actor : sActors)
    {
      delete actor;
      actor = nullptr;
    }

    sActors.clear();
  }

  Actor* Scene::GetMainActor()
  {
    return sMainActor;
  }

  Camera* Scene::GetMainCamera()
  {
    if (sMainActor)
    {
      return sMainActor->GetComponent<Camera>();
    }

    return nullptr;
  }

  void Scene::DestroyActor(Actor* Actor)
  {
    auto actorIt = std::find(sActors.begin(), sActors.end(), Actor);

    if (actorIt != sActors.end())
    {
      delete *actorIt;
      *actorIt = nullptr;

      sActors.erase(actorIt);
    }
  }

  void Scene::Update(R32 TimeDelta)
  {
    for (const auto& actor : sActors)
    {
      actor->Update(TimeDelta);

      Transform* transform = actor->GetTransform();
      Renderable* renderable = actor->GetComponent<Renderable>();

      if (transform && renderable)
      {
        DefaultRenderer::AddRenderTask(RenderTask{ transform, renderable->GetMeshPtr() });
      }

      if (actor != sMainActor)
      {
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight() * 2.0F, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp() * 2.0F, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront() * 2.0F, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight() * 2.0F, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp() * 2.0F, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront() * 2.0F, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        if (actor->GetParent())
        {
          DebugRenderer::DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition(), R32V4{ 1.0F, 1.0F, 1.0F, 1.0F });
        }

        DebugRenderer::DebugBox(transform->GetWorldPosition(), transform->GetWorldScale(), R32V4{ 1.0F, 1.0F, 0.0F, 1.0F }, transform->GetQuaternion());
      }
    }
  }

  void Scene::Serialize()
  {

  }

  void Scene::DeSerialize()
  {
    for (const auto file : fs::directory_iterator{ fs::path{ gConfig["unpackDir"].GetString() } / sRegionId / sLevelId })
    {
      if (file.path().extension() == ".TSC") ObjectSerializer{ file };
      if (file.path().extension() == ".TRE") ObjectSerializer{ file };
      if (file.path().extension() == ".TAT") ObjectSerializer{ file };

      if (file.path().extension() == ".SCR") ModelSerializer{ file };
    }
  }
}