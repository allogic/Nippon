#pragma once

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
    inline void SetPosition(const R32V3& Value) { mPosition = Value; }
    inline void SetRotation(const R32V3& Value) { mRotation = Value; }
    inline void SetScale(const R32V3& Value) { mScale = Value; }

  private:

    U8 mId = {};
    U8 mCategory = {};
    R32V3 mPosition = {};
    R32V3 mRotation = {};
    R32V3 mScale = {};
  };
}