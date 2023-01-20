#pragma once

#include <string>
#include <vector>

#include <Common/Common.h>
#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Model
  {
  public:

    struct Vertex
    {
      PackedTuple<I16> Position;
      PackedPair<U16> TextureMap;
      PackedPair<U16> TextureUv;
      U32 ColorWeight;
    };

    struct Transform
    {
      PackedTuple<I16> Position;
      PackedTuple<I16> Rotation;
      PackedTuple<U16> Scale;
    };

  public:

    inline const auto& GetName() const { return mName; }
    inline const auto& GetTransform() const { return mTransform; }
    inline const auto& GetVertexBuffer() const { return mVertexBuffer; }
    inline const auto& GetElementBuffer() const { return mElementBuffer; }

  public:

    inline void SetName(const std::string& Value) { mName = Value; }
    inline void SetTransform(const Transform& Value) { mTransform = Value; }

    inline void AddVertex(const Vertex& Value) { mVertexBuffer.emplace_back(Value); }
    inline void AddElement(U16 Value) { mElementBuffer.emplace_back(Value); }

  private:

    std::string mName = {};
    Transform mTransform = {};
    std::vector<Vertex> mVertexBuffer = {};
    std::vector<U32> mElementBuffer = {};
  };
}