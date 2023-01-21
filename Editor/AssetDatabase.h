#pragma once

#include <vector>

#include <Editor/Assets/Model.h>
#include <Editor/Assets/Object.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class AssetDatabase
  {
  public:

    static const auto& GetModels() { return mModels; }
    static const auto& GetObjects() { return mObjects; }

  public:

    static inline void ClearModels() { mModels.clear(); }
    static inline void ClearObjects() { mObjects.clear(); }

  public:

    static inline void AddModel(const Model& Value) { mModels.emplace_back(Value); }
    static inline void AddObject(const Object& Value) { mObjects.emplace_back(Value); }

  private:

    static inline std::vector<Model> mModels = {};
    static inline std::vector<Object> mObjects = {};
  };
}