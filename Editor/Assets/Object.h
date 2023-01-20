#pragma once

#include <Common/Common.h>
#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Object
  {
  public:

    inline auto GetId() const { return mId; }
    inline auto GetCategory() const { return mCategory; }
    inline const auto& GetPosition() const { return mPosition; }
    inline const auto& GetRotation() const { return mRotation; }
    inline const auto& GetScale() const { return mScale; }

  public:

    inline void SetId(U8 Value) { mId = Value; }
    inline void SetCategory(U8 Value) { mCategory = Value; }
    inline void SetPosition(const PackedTuple<U16>& Value) { mPosition = Value; }
    inline void SetRotation(const PackedTuple<U8>& Value) { mRotation = Value; }
    inline void SetScale(const PackedTuple<U8>& Value) { mScale = Value; }

  private:

    U8 mId = 0;
    U8 mCategory = 0;
    PackedTuple<U16> mPosition = {};
    PackedTuple<U8> mRotation = {};
    PackedTuple<U8> mScale = {};
  };
}