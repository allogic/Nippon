#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <utility>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>
#include <Editor/Header.h>
#include <Editor/FrameBuffer.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Serializer/ScrSerializer.h>
#include <Editor/Serializer/TblSerializer.h>
#include <Editor/Serializer/ItsSerializer.h>

#include <Vendor/rapidjson/rapidjson.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;
namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Scene
  {
  public:

    Scene(const std::string& Region, const std::string& Level);
    ~Scene();

  public:

    static void Switch(const std::string& Region, const std::string& Level);

  public:

    inline auto GetWidth() const { return mWidth; }
    inline auto GetHeight() const { return mHeight; }

    inline const auto& GetObjects() const { return mObj; }
    inline const auto& GetModels() const { return mModels; }
    inline const auto& GetTextures() const { return mTextures; }
    inline const auto& GetActors() const { return mActors; }

    inline const auto& GetFrameBuffer() const { return mFrameBuffer; }

    inline auto& GetDebugRenderer() { return mDebugRenderer; }
    inline auto& GetDefaultRenderer() { return mDefaultRenderer; }

  public:

    Actor* GetMainActor();
    Camera* GetMainCamera();

  public:

    template<typename A, typename ... Args>
    A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

    void DestroyActor(Actor* Actor);

  public:

    void Resize(U32 Width, U32 Height);
    void Update(R32 TimeDelta);
    void Render();

    void SubmitRenderTasks();
    void DoSelectionRecursive(Actor* Actor);

  private:

    void Serialize();
    void DeSerialize();

  private:

    void ModelsToActors();
    void ObjectsToActors();

  private:

    std::string mRegion;
    std::string mLevel;

    U32 mWidth = 0;
    U32 mHeight = 0;

    std::vector<Actor*> mActors = {};
    Actor* mMainActor = nullptr;

    std::string mMapId = "";

    fs::path mLvlDir = "";

    fs::path mDatDir = "";
    fs::path mBinDir = "";
    fs::path mBinFDir = "";
    fs::path mBinGDir = "";
    fs::path mBinJDir = "";

    fs::path mScpDir = "";
    fs::path mDdpDir = "";

    fs::path mTscFile = "";
    fs::path mTreFile = "";
    fs::path mTatFile = "";
    fs::path mItsFile = "";

    std::vector<fs::path> mScrFiles = {};
    std::vector<fs::path> mDdsFiles = {};

    std::vector<ObjEntry> mObj = {};
    std::vector<ItsEntry> mIts = {};

    std::vector<std::pair<Model, ScrTransform>> mModels = {};
    std::vector<Texture2D*> mTextures = {};

    DebugRenderer mDebugRenderer = { 65535, 65535 * 2 };
    DefaultRenderer mDefaultRenderer = {};

    FrameBuffer mFrameBuffer = {};
  };
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  template<typename A, typename ... Args>
  A* Scene::CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments)
  {
    auto actor = mActors.emplace_back(new A{ Name, std::forward<Args>(Arguments) ... });
    if (Parent)
    {
      actor->SetParent(Parent);
      Parent->AddChild(actor);
    }
    return (A*)actor;
  }
}