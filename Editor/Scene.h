#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <utility>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>
#include <Editor/Header.h>
#include <Editor/Model.h>
#include <Editor/FrameBuffer.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

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

    Scene(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
    virtual ~Scene();

  public:

    inline const auto& GetName() const { return mName; }
    inline const auto& GetWidth() const { return mWidth; }
    inline const auto& GetHeight() const { return mHeight; }
    inline const auto& GetActors() const { return mActors; }
    inline const auto& GetModels() const { return mModels; }
    inline const auto& GetTextures() const { return mTextures; }
    inline const auto& GetFrameBuffer() const { return mFrameBuffer; }
    inline const auto& GetMainActor() const { return mMainActor; }
    inline const auto& GetMainCamera() const { return mMainCamera; }
    inline const auto& GetViewport() const { return mViewport; }

    inline auto& GetDebugRenderer() { return mDebugRenderer; }
    inline auto& GetDefaultRenderer() { return mDefaultRenderer; }

  public:

    template<typename A, typename ... Args>
    A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

    void DestroyActor(Actor* Actor);

  public:

    void Resize(U32 Width, U32 Height);
    void Update();
    void Render();

    void SubmitRenderTasks();
    void DoSelectionRecursive(Actor* Actor);

  protected:

    virtual void Load() = 0;
    virtual void Save() = 0;

  protected:

    std::string mEntry;
    std::string mSubEntry;
    std::string mName;
    Viewport* mViewport;

    Actor* mMainActor = nullptr;
    Camera* mMainCamera = nullptr;

    std::vector<Actor*> mActors = {};
    std::vector<std::pair<Model, ScrTransform>> mModels = {};
    std::vector<Texture2D*> mTextures = {};

    U32 mWidth = 1;
    U32 mHeight = 1;

    DebugRenderer mDebugRenderer = { this, 65535, 65535 * 2 };
    DefaultRenderer mDefaultRenderer = { this };

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
    auto actor = mActors.emplace_back(new A{ this, Name, std::forward<Args>(Arguments) ... });
    if (Parent)
    {
      actor->SetParent(Parent);
      Parent->AddChild(actor);
    }
    return (A*)actor;
  }
}