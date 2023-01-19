#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Editor/Actor.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  namespace fs = std::filesystem;

  class Camera;

  class Scene
  {
  public:

    Scene(const std::string& RegionId, const std::string& LevelId);
    virtual ~Scene();

  public:

    inline auto begin() { return mActors.begin(); }
    inline const auto begin() const { return mActors.cbegin(); }

    inline auto end() { return mActors.end(); }
    inline const auto end() const { return mActors.cend(); }

  public:

    template<typename A, typename ... Args>
    A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

    void DestroyActor(Actor* Actor);

  public:

    Actor* GetMainActor();
    Camera* GetMainCamera();

  public:

    inline void SetMainActor(Actor* Actor) { mMainActor = Actor; }

  private:

    void Serialize();
    void DeSerialize();

  private:

    std::string mRegionId;
    std::string mLevelId;

    std::vector<Actor*> mActors = {};
    Actor* mMainActor = nullptr;
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