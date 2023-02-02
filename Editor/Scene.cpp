#include <Common/Debug.h>
#include <Common/ExtensionIterator.h>

#include <Common/Utils/StringUtils.h>
#include <Common/Utils/FileUtils.h>

#include <Editor/Scene.h>

#include <Editor/Actors/Player.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Vendor/rapidjson/document.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Scene::Scene(const std::string& Directory, const std::string& SubDirectory, const std::string& Type)
    : mDirectory{ Directory }
    , mSubDirectory{ SubDirectory }
    , mType{ Type }
  {
    LOG("Opening scene /%s/%s\n", mDirectory.c_str(), mSubDirectory.c_str());

    DeSerialize();

    mMainActor = CreateActor<Player>("Player", nullptr);

    ModelsToActors();
    ObjectsToActors();
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
    mObjects.clear();
    mModels.clear();

    LOG("Closing scene /%s/%s\n", mDirectory.c_str(), mSubDirectory.c_str());
    LOG("\n");
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

  void Scene::Resize(U32 Width, U32 Height)
  {
    mWidth = Width;
    mHeight = Height;

    mFrameBuffer.Resize(mWidth, mHeight);
  }

  void Scene::Update(R32 TimeDelta)
  {
    mDebugRenderer.DebugLine(R32V3{ -10000.0F, 0.0F, 0.0F }, R32V3{ 10000.0F, 0.0F, 0.0F }, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
    mDebugRenderer.DebugLine(R32V3{ 0.0F, -10000.0F, 0.0F }, R32V3{ 0.0F, 10000.0F, 0.0F }, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
    mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, -10000.0F }, R32V3{ 0.0F, 0.0F, 10000.0F }, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

    for (const auto& actor : mActors)
    {
      actor->Update(TimeDelta);

      Transform* transform = actor->GetTransform();
      Renderable* renderable = actor->GetComponent<Renderable>();

      if (transform && renderable)
      {
        mDefaultRenderer.AddRenderTask(RenderTask{ transform, renderable->GetMeshPtr() });
      }

      if (actor != mMainActor)
      {
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        if (actor->GetParent())
        {
          mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition(), R32V4{ 1.0F, 1.0F, 1.0F, 1.0F });
        }

        mDebugRenderer.DebugBox(transform->GetWorldPosition(), transform->GetWorldScale(), R32V4{ 1.0F, 1.0F, 0.0F, 1.0F }, transform->GetQuaternion());
      }
    }
  }

  void Scene::Render()
  {
    glViewport(0, 0, (I32)mWidth, (I32)mHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer.GetId());

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    mDefaultRenderer.Render();

    glDisable(GL_CULL_FACE);

    mDebugRenderer.Render();

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  void Scene::Serialize()
  {

  }

  void Scene::DeSerialize()
  {
    fs::path targetDir = fs::path{ gConfig["unpackDir"].GetString() } / mDirectory / mSubDirectory;

    if (mType == "map")
    {
      std::string mapId = StringUtils::CutFront(mDirectory, 2);

      auto tscObjects = ObjectSerializer::ToObjects(FileUtils::ReadBinary(targetDir.string() + "/r" + mapId + mSubDirectory + "_objtbl.TSC"));
      auto treObjects = ObjectSerializer::ToObjects(FileUtils::ReadBinary(targetDir.string() + "/r" + mapId + mSubDirectory + "_objtbl2.TRE"));
      auto tatObjects = ObjectSerializer::ToObjects(FileUtils::ReadBinary(targetDir.string() + "/r" + mapId + mSubDirectory + "_objtbl3.TAT"));

      mObjects.insert(mObjects.end(), tscObjects.begin(), tscObjects.end());
      mObjects.insert(mObjects.end(), treObjects.begin(), treObjects.end());
      mObjects.insert(mObjects.end(), tatObjects.begin(), tatObjects.end());

      for (const auto& file : ExtensionIterator{ targetDir / "SCP", { ".SCR" } })
      {
        auto models = ModelSerializer::ToModels(FileUtils::ReadBinary(file.string()));

        mModels.insert(mModels.end(), models.begin(), models.end());
      }
    }
    else if (mType == "character")
    {
      //for (const auto& file : ExtensionIterator{ targetDir, { ".MD" } })
      //{
      //  ModelParser scrParser{ mMdDatabase, file };
      //}
    }
    else if (mType == "item")
    {
      //for (const auto& file : ExtensionIterator{ targetDir, { ".MD" } })
      //{
      //  ModelParser scrParser{ mMdDatabase, file };
      //}
    }
  }

  void Scene::ModelsToActors()
  {
    for (const auto& [model, transform] : mModels)
    {
      Actor* modelActor = CreateActor<Actor>("Model", nullptr);

      Transform* modelTransform = modelActor->GetTransform();

      modelTransform->SetWorldPosition(R32V3{ transform.Position.x, transform.Position.y, transform.Position.z });
      modelTransform->SetWorldRotation(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z });
      modelTransform->SetWorldScale(R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z });

      for (const auto& division : model.Divisions)
      {
        Actor* divisionActor = CreateActor<Actor>("Division", modelActor);

        Renderable* divisionRenderable = divisionActor->AttachComponent<Renderable>();

        divisionRenderable->SetVertexBuffer(VertexConverter::ToVertexBuffer(division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights));
        divisionRenderable->SetElementBuffer(ElementConverter::ToElementBuffer(division.Vertices));
      }
    }
  }

  void Scene::ObjectsToActors()
  {
    for (const auto& object : mObjects)
    {
      Actor* objectActor = CreateActor<Actor>("Object", nullptr);
    
      Transform* objectTransform = objectActor->GetTransform();

      objectTransform->SetWorldPosition(R32V3{ object.Position.x, object.Position.y, object.Position.z });
      objectTransform->SetWorldRotation(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z });
      objectTransform->SetWorldScale(R32V3{ object.Scale.x, object.Scale.y, object.Scale.z });
    }
  }
}