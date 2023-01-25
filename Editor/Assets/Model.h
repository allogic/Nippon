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
  class ModelDivision
  {
  public:

    inline const auto& GetVertexBuffer() const { return mVertexBuffer; }
    inline const auto& GetElementBuffer() const { return mElementBuffer; }

    inline auto GetVertexCount() const { return mVertexBuffer.size(); }
    inline auto GetElementCount() const { return mElementBuffer.size(); }

  public:

    inline void AddVertex(const DefaultVertex& Value) { mVertexBuffer.emplace_back(Value); }
    inline void AddElement(U16 Value) { mElementBuffer.emplace_back(Value); }

  private:

    std::vector<DefaultVertex> mVertexBuffer = {};
    std::vector<U32> mElementBuffer = {};
  };

  class ModelEntry
  {
  public:

    ModelEntry(U32 Id, U32 Type);

  public:

    inline auto GetId() const { return mId; }
    inline auto GetType() const { return mType; }

    inline const auto& GetPosition() const { return mPosition; }
    inline const auto& GetRotation() const { return mRotation; }
    inline const auto& GetScale() const { return mScale; }

    inline const auto& GetDivisions() const { return mDivisions; }

    inline auto GetDivisionCount() const { return mDivisions.size(); }

  public:

    inline void SetPosition(const R32V3& Value) { mPosition = Value; }
    inline void SetRotation(const R32V3& Value) { mRotation = Value; }
    inline void SetScale(const R32V3& Value) { mScale = Value; }

  public:

    inline void AddDivision(const ModelDivision& Value) { mDivisions.emplace_back(Value); }

  public:

    inline auto begin() { return mDivisions.begin(); }
    inline const auto begin() const { return mDivisions.cbegin(); }

    inline auto end() { return mDivisions.end(); }
    inline const auto end() const { return mDivisions.cend(); }

  public:

    auto& operator [] (U64 Index) { return mDivisions[Index]; }
    const auto& operator [] (U64 Index) const { return mDivisions[Index]; }

  private:

    U32 mId;
    U32 mType;

    R32V3 mPosition = {};
    R32V3 mRotation = {};
    R32V3 mScale = {};

    std::vector<ModelDivision> mDivisions = {};
  };

  class ModelGroup
  {
  public:

    ModelGroup(const std::string& Name);

  public:

    inline const auto& GetName() const { return mName; }
    inline const auto& GetEntries() const { return mEntries; }

    inline auto GetEntryCount() const { return mEntries.size(); }

  public:

    inline void AddEntry(const ModelEntry& Value) { mEntries.emplace_back(Value); }

  public:

    inline auto begin() { return mEntries.begin(); }
    inline const auto begin() const { return mEntries.cbegin(); }

    inline auto end() { return mEntries.end(); }
    inline const auto end() const { return mEntries.cend(); }

  public:

    auto& operator [] (U64 Index) { return mEntries[Index]; }
    const auto& operator [] (U64 Index) const { return mEntries[Index]; }

  private:

    std::string mName;

    std::vector<ModelEntry> mEntries = {};
  };
}