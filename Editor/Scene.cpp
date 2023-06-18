#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Scene.h>
#include <Editor/Texture.h>

#include <Editor/Actors/Player.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Interface/Scene/Outline.h>

#include <Vendor/rapidjson/document.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

extern ark::Scene* gScene;

extern ark::Outline* gOutline;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Scene::Scene(const std::string& Region, const std::string& Level)
    : mRegion{ Region }
    , mLevel{ Level }
    , mMapId{ StringUtils::CutFront(Region, 2) }
  {
    mLvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mRegion / mLevel;
    mDatDir = mLvlDir / fs::path{ "r" + mMapId + mLevel + ".dat" };

    mScpDir = FsUtils::SearchFileByType(mDatDir, "SCP");
    mDdpDir = FsUtils::SearchFileByType(mScpDir, "DDP");

    mTscFile = FsUtils::SearchFileByType(mDatDir, "TSC");
    mTreFile = FsUtils::SearchFileByType(mDatDir, "TRE");
    mTatFile = FsUtils::SearchFileByType(mDatDir, "TAT");

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

    gOutline->Reset();

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
        mDefaultRenderer.AddRenderTask(RenderTask{ transform, &renderable->GetMesh(), renderable->GetTexture() });
      }

      if (actor != mMainActor)
      {
        mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        if (actor->GetParent())
        {
          mDebugRenderer.DebugLine(transform->GetPosition(), transform->GetPosition(), R32V4{ 1.0F, 1.0F, 1.0F, 1.0F });
        }

        mDebugRenderer.DebugBox(transform->GetPosition(), transform->GetScale(), R32V4{ 1.0F, 1.0F, 0.0F, 1.0F }, transform->GetQuaternion());
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
    auto tscObjects = ObjectSerializer::FromFile(mTscFile);
    auto treObjects = ObjectSerializer::FromFile(mTreFile);
    auto tatObjects = ObjectSerializer::FromFile(mTatFile);

    mObjects.insert(mObjects.end(), tscObjects.begin(), tscObjects.end());
    mObjects.insert(mObjects.end(), treObjects.begin(), treObjects.end());
    mObjects.insert(mObjects.end(), tatObjects.begin(), tatObjects.end());

    mScrFiles = FsUtils::SearchFilesByTypeRecursive(mDatDir, "SCR");
    mDdsFiles = FsUtils::SearchFilesByTypeRecursive(mDatDir, "DDS");

    for (const auto& file : mScrFiles)
    {
      auto models = ModelSerializer::FromFile(file);

      mModels.insert(mModels.end(), models.begin(), models.end());
    }

    for (const auto& file : mDdsFiles)
    {
      mTextures.emplace_back(TextureUtils::LoadDDS(file));
    }
  }

  void Scene::ModelsToActors()
  {
    for (const auto& [model, transform] : mModels)
    {
      Actor* modelActor = CreateActor<Actor>(model.Name, nullptr);

      Transform* modelTransform = modelActor->GetTransform();

      modelTransform->SetPosition(R32V3{ transform.Position.x, transform.Position.y, transform.Position.z });
      modelTransform->SetRotation(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z });
      modelTransform->SetScale(R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z });

      for (const auto& division : model.Divisions)
      {
        Actor* divisionActor = CreateActor<Actor>("Division", modelActor);

        Renderable* divisionRenderable = divisionActor->AttachComponent<Renderable>();

        divisionRenderable->SetVertexBuffer(VertexConverter::ToVertexBuffer(division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights));
        divisionRenderable->SetElementBuffer(ElementConverter::ToElementBuffer(division.Vertices));
        divisionRenderable->SetTexture((division.Header.TextureIndex < mTextures.size()) ? mTextures[division.Header.TextureIndex] : nullptr);
      }
    }
  }

  void Scene::ObjectsToActors()
  {
    for (const auto& object : mObjects)
    {
      Actor* objectActor = CreateActor<Actor>("Object", nullptr);
    
      Transform* objectTransform = objectActor->GetTransform();

      objectTransform->SetPosition(R32V3{ object.Position.x, object.Position.y, object.Position.z });
      objectTransform->SetRotation(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z });
      objectTransform->SetScale(R32V3{ object.Scale.x, object.Scale.y, object.Scale.z });
    }
  }
}