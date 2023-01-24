#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Vertex.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Model
  {
  public:

    struct Transform
    {
      R32V3 Position;
      R32V3 Rotation;
      R32V3 Scale;
    };

  public:

    inline const auto& GetName() const { return mName; }
    inline const auto& GetTransform() const { return mTransform; }
    inline const auto& GetVertexBuffer() const { return mVertexBuffer; }
    inline const auto& GetElementBuffer() const { return mElementBuffer; }

  public:

    inline void SetName(const std::string& Value) { mName = Value; }
    inline void SetTransform(const Transform& Value) { mTransform = Value; }

    inline void AddVertex(const DefaultVertex& Value) { mVertexBuffer.emplace_back(Value); }
    inline void AddElement(U16 Value) { mElementBuffer.emplace_back(Value); }

  private:

    std::string mName = "";
    Transform mTransform = {};
    std::vector<DefaultVertex> mVertexBuffer = {};
    std::vector<U32> mElementBuffer = {};
  };
}