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
  class LevelScene : public Scene
  {
  public:

    static void Create(const std::string& Entry, const std::string& SubEntry, const std::string& Name, std::vector<LevelScene*>& Scenes);

  public:

    LevelScene(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
    virtual ~LevelScene();

  public:

    inline const auto& GetObjects() const { return mObjects; }

  protected:

    virtual void Load() override;
    virtual void Save() override;

  private:

    void ModelsToActors();
    void ObjectsToActors();

  private:

    std::vector<ObjEntry> mObjects = {};
  };
}