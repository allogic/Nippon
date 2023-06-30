#pragma once

#include <string>
#include <map>

#include <Common/Types.h>

#include <Editor/Forward.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class SceneManager
  {
  public:

    static Scene* GetActiveScene();
    static void SetActiveScene(Scene* Scene);

    static void CreateLevel(const std::string& Entry, const std::string& SubEntry, rj::Value& Value);
    static void CreateEntity(const std::string& Entry, const std::string& SubEntry, rj::Value& Value);

    static void Draw();

    static void Destroy(Scene* Scene);
  };
}