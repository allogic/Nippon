#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Math.h>
#include <Editor/Scene.h>
#include <Editor/Texture.h>
#include <Editor/Math.h>

#include <Editor/Actors/Box.h>
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

#include <Vendor/GLFW/glfw3.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern GLFWwindow* gGlfwContext;

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
    std::string windowTitle = "Nippon - /" + mRegion + "/" + mLevel;

    glfwSetWindowTitle(gGlfwContext, windowTitle.c_str());

    mLvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mRegion / mLevel;

    mDatDir = mLvlDir / fs::path{ "r" + mMapId + mLevel + ".dat" };
    mBinDir = mLvlDir / fs::path{ "r" + mMapId + mLevel + ".bin" };
    mBinFDir = mLvlDir / fs::path{ "r" + mMapId + mLevel + "_f.bin" };
    mBinGDir = mLvlDir / fs::path{ "r" + mMapId + mLevel + "_g.bin" };
    mBinJDir = mLvlDir / fs::path{ "r" + mMapId + mLevel + "_j.bin" };

    mScpDir = FsUtils::SearchFileByType(mDatDir, "SCP");
    mDdpDir = FsUtils::SearchFileByType(mScpDir, "DDP");
    
    mTscFile = FsUtils::SearchFileByType(mDatDir, "TSC");
    mTreFile = FsUtils::SearchFileByType(mDatDir, "TRE");
    mTatFile = FsUtils::SearchFileByType(mDatDir, "TAT");
    mItsFile = FsUtils::SearchFileByType(mBinDir, "ITS");

    DeSerialize();

    mMainActor = CreateActor<Player>("Player", nullptr);

    ModelsToActors();
    //ObjectsToActors(); // TODO: fix this!
  }

  Scene::~Scene()
  {
    Serialize();

    for (auto& actor : mActors)
    {
      delete actor;
      actor = nullptr;
    }

    for (auto& texture : mTextures)
    {
      delete texture;
      texture = nullptr;
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
    mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1000.0F, 0.0F, 0.0F }, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
    mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1000.0F, 0.0F }, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
    mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1000.0F }, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

    for (const auto& actor : mActors)
    {
      actor->Update(TimeDelta);
    }

    SubmitRenderTasks();
    DoSelectionRecursive(gOutline->GetSelectedActor());
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

  void Scene::SubmitRenderTasks()
  {
    for (const auto& actor : mActors)
    {
      if (actor->IsActive())
      {
        Transform* transform = actor->GetTransform();
        Renderable* renderable = actor->GetComponent<Renderable>();

        if (transform && renderable)
        {
          mDefaultRenderer.AddRenderTask(RenderTask{ transform, &renderable->GetMesh(), renderable->GetTexture() });
        }
      }
    }
  }

  void Scene::DoSelectionRecursive(Actor* Actor)
  {
    if (Actor && Actor != mMainActor)
    {
      Transform* transform = Actor->GetComponent<Transform>();

      if (transform)
      {
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
        mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

        mDebugRenderer.DebugAxisAlignedBoundingBox(Actor->GetAABB(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });

        for (const auto& child : Actor->GetChildren())
        {
          DoSelectionRecursive(child);
        }
      }
    }
  }

  void Scene::Serialize()
  {

  }

  void Scene::DeSerialize()
  {
    auto tsc = TblSerializer::FromFile(mTscFile);
    auto tre = TblSerializer::FromFile(mTreFile);
    auto tat = TblSerializer::FromFile(mTatFile);

    mObj.insert(mObj.end(), tsc.begin(), tsc.end());
    mObj.insert(mObj.end(), tre.begin(), tre.end());
    mObj.insert(mObj.end(), tat.begin(), tat.end());

    mIts = ItsSerializer::FromFile(mItsFile);

    mScrFiles = FsUtils::SearchFilesByTypeRecursive(mDatDir, "SCR");
    mDdsFiles = FsUtils::SearchFilesByTypeRecursive(mDatDir, "DDS");

    for (const auto& file : mScrFiles)
    {
      auto models = ScrSerializer::FromFile(file);

      mModels.insert(mModels.end(), models.begin(), models.end());
    }

    for (const auto& file : mDdsFiles)
    {
      mTextures.emplace_back(TextureUtils::LoadDDS(file));
    }
  }

  void Scene::ModelsToActors()
  {
    for (const auto& [model, trans] : mModels)
    {
      Actor* modelActor = CreateActor<Actor>(model.Name, nullptr);

      Transform* transform = modelActor->GetTransform();

      transform->SetLocalPosition(R32V3{ trans.Position.x, trans.Position.y, trans.Position.z });
      transform->SetLocalRotation(glm::degrees(R32V3{ 0.0F, trans.Rotation.y, 0.0F } / 360.0F));
      transform->SetLocalScale(R32V3{ trans.Scale.x, trans.Scale.y, trans.Scale.z } / 100000.0F);

      for (const auto& division : model.Divisions)
      {
        Actor* childActor = CreateActor<Actor>("Division", modelActor);

        Renderable* renderable = childActor->AttachComponent<Renderable>();

        std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
        std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);
        Texture2D* texture = (division.Header.TextureIndex < mTextures.size()) ? mTextures[division.Header.TextureIndex] : nullptr;

        renderable->SetVertexBuffer(vertices);
        renderable->SetElementBuffer(elements);
        renderable->SetTexture(texture);

        AABB aabb = Math::ComputeBoundingBox(vertices, transform->GetLocalScale());

        childActor->SetAABB(aabb);
      }
    }
  }

  void Scene::ObjectsToActors()
  {
    for (const auto& obj : mObj)
    {
      Actor* objActor = CreateActor<Actor>("Object", nullptr);
    
      Transform* objTransform = objActor->GetTransform();
    
      objTransform->SetLocalPosition(R32V3{ obj.Position.x, obj.Position.y, obj.Position.z });
      objTransform->SetLocalRotation(R32V3{ obj.Rotation.x, obj.Rotation.y, obj.Rotation.z } / 255.0F);
      objTransform->SetLocalScale(R32V3{ obj.Scale.x, obj.Scale.y, obj.Scale.z } / 100000.0F);
    }
  }
}