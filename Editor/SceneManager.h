#pragma once

#include <string>
#include <map>

#include <Common/Types.h>

#include <Editor/Forward.h>

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

    static void CreateLevel(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
    static void CreateEntity(const std::string& Entry, const std::string& SubEntry, const std::string& Name);

    static void DrawAllViewports();

    static void Destroy(Scene* Scene);
  };
}