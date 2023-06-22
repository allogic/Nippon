#pragma once

#include <string>
#include <vector>
#include <utility>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Header.h>
#include <Editor/Model.h>
#include <Editor/Scene.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class EntityScene : public Scene
  {
  public:

    static void Create(const std::string& Entry, const std::string& SubEntry, const std::string& Name, std::vector<EntityScene*>& Scenes);

  public:

    EntityScene(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
    virtual ~EntityScene();

  protected:

    virtual void Load() override;
    virtual void Save() override;
  };
}