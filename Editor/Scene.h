#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <utility>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>
#include <Editor/FrameBuffer.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Serializer/ModelSerializer.h>
#include <Editor/Serializer/ObjectSerializer.h>

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

    Scene(const std::string& Directory, const std::string& SubDirectory, const std::string& Type);
    ~Scene();

  public:

    inline auto GetWidth() const { return mWidth; }
    inline auto GetHeight() const { return mHeight; }

    inline const auto& GetObjects() const { return mObjects; }
    inline const auto& GetModels() const { return mModels; }
    inline const auto& GetActors() const { return mActors; }

    inline const auto& GetFrameBuffer() const { return mFrameBuffer; }

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

  private:

    void Serialize();
    void DeSerialize();

  private:

    void ModelsToActors();
    void ObjectsToActors();

  private:

    std::string mDirectory;
    std::string mSubDirectory;
    std::string mType;

    U32 mWidth = 0;
    U32 mHeight = 0;

    std::vector<Actor*> mActors = {};
    Actor* mMainActor = nullptr;

    std::vector<Object> mObjects = {};
    std::vector<std::pair<Model, ScrTransform>> mModels = {};

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