#include <Common/Utils/ArchiveUtils.h>
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

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Scene::Scene(const std::string& Region, const std::string& Level)
    : mRegion{ Region }
    , mLevel{ Level }
  {
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
  }

  void Scene::Switch(const std::string& Region, const std::string& Level)
  {
    U32 prevWidth = 1;
    U32 prevHeight = 1;

    if (gScene)
    {
      prevWidth = gScene->GetWidth();
      prevHeight = gScene->GetHeight();

      delete gScene;
      gScene = nullptr;
    }

    gScene = new Scene{ Region, Level };

    if (gScene)
    {
      gScene->Resize(prevWidth, prevHeight);
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
    fs::path targetDir = fs::path{ gConfig["unpackDir"].GetString() } / mRegion / mLevel;

    std::string mapId = StringUtils::CutFront(mRegion, 2);

    std::string tscFile = targetDir.string() + "/r" + mapId + mLevel + ".dat" + "/00000@r" + mapId + mLevel + "_objtbl@TSC";
    std::string treFile = targetDir.string() + "/r" + mapId + mLevel + ".dat" + "/00001@r" + mapId + mLevel + "_objtbl2@TRE";
    std::string tatFile = targetDir.string() + "/r" + mapId + mLevel + ".dat" + "/00002@r" + mapId + mLevel + "_objtbl3@TAT";

    auto tscObjects = ObjectSerializer::ToObjects(FileUtils::ReadBinary(tscFile));
    auto treObjects = ObjectSerializer::ToObjects(FileUtils::ReadBinary(treFile));
    auto tatObjects = ObjectSerializer::ToObjects(FileUtils::ReadBinary(tatFile));

    mObjects.insert(mObjects.end(), tscObjects.begin(), tscObjects.end());
    mObjects.insert(mObjects.end(), treObjects.begin(), treObjects.end());
    mObjects.insert(mObjects.end(), tatObjects.begin(), tatObjects.end());

    std::string scrDir = targetDir.string() + "/r" + mapId + mLevel + ".dat" + "/00005@r" + mapId + mLevel + "@SCP";

    for (const auto& file : fs::directory_iterator{ scrDir })
    {
      U16 index = 0;
      std::string name = "";
      std::string type = "";

      ArchiveUtils::FromArchiveName(file.path().stem().string(), index, name, type);

      if (type == "SCR")
      {
        auto models = ModelSerializer::ToModels(FileUtils::ReadBinary(file.path()));

        mModels.insert(mModels.end(), models.begin(), models.end());
      }
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