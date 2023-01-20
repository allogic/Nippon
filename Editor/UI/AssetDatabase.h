#pragma once

#include <vector>

#include <Editor/UI.h>

#include <Editor/Assets/Model.h>
#include <Editor/Assets/Object.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class AssetDatabase : public UI
  {
  public:

    virtual void Update() override;
    virtual void Draw() override;

  public:

    inline void AddModel(const Model& Value) { mModels.emplace_back(Value); }
    inline void AddObject(const Object& Value) { mObjects.emplace_back(Value); }

  private:

    std::vector<Model> mModels = {};
    std::vector<Object> mObjects = {};
  };
}