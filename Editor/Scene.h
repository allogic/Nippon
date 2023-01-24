#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>

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

    static void Create(const std::string& RegionId, const std::string& LevelId);
    static void Destroy();

  public:

    static inline const auto& GetActors() { return sActors; }
    static Actor* GetMainActor();
    static Camera* GetMainCamera();

  public:

    template<typename A, typename ... Args>
    static A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

    static void DestroyActor(Actor* Actor);

  public:

    static void Update(R32 TimeDelta);

  private:

    static void Serialize();
    static void DeSerialize();

  private:

    static inline std::string sRegionId = "";
    static inline std::string sLevelId = "";
    static inline std::vector<Actor*> sActors = {};
    static inline Actor* sMainActor = nullptr;
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
    auto actor = sActors.emplace_back(new A{ Name, std::forward<Args>(Arguments) ... });
    if (Parent)
    {
      actor->SetParent(Parent);
      Parent->AddChild(actor);
    }
    return (A*)actor;
  }
}