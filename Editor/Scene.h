#pragma once

#pragma warning(disable : 26444)

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>

#include <Editor/Assets/Model.h>
#include <Editor/Assets/Object.h>

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

    inline const auto& GetObjects() const { return mObjects; }
    inline const auto& GetModelGroups() const { return mModelGroups; }

    inline const auto& GetActors() { return mActors; }

    Actor* GetMainActor();
    Camera* GetMainCamera();

  public:

    inline void AddObject(const Object& Value) { mObjects.emplace_back(Value); }
    inline void AddModelGroup(const ModelGroup& Value) { mModelGroups.emplace_back(Value); }

  public:

    template<typename A, typename ... Args>
    A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

    void DestroyActor(Actor* Actor);

  public:

    void Update(R32 TimeDelta);

  private:

    void Serialize();
    void DeSerialize();

  private:

    std::string mDirectory;
    std::string mSubDirectory;
    std::string mType;

    std::vector<Actor*> mActors = {};
    Actor* mMainActor = nullptr;

    std::vector<Object> mObjects = {};
    std::vector<ModelGroup> mModelGroups = {};
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