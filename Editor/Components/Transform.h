#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Component.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Transform : public Component
  {
  public:

    Transform(Actor* Actor);

  public:

    inline const auto& GetWorldRight() const { return mWorldRight; }
    inline const auto& GetWorldUp() const { return mWorldUp; }
    inline const auto& GetWorldFront() const { return mWorldFront; }

    inline const auto& GetLocalRight() const { return mLocalRight; }
    inline const auto& GetLocalUp() const { return mLocalUp; }
    inline const auto& GetLocalFront() const { return mLocalFront; }

  public:

    R32M4 GetModelMatrix() const;

  public:

    R32V3 GetPosition() const;
    R32V3 GetRotation() const;
    R32Q GetQuaternion() const;
    R32V3 GetScale() const;

  public:

    void SetPosition(const R32V3& Position);
    void SetRotation(const R32V3& Rotation);
    void SetScale(const R32V3& Scale);

  public:

    void AddPosition(const R32V3& Position);
    void AddRotation(const R32V3& Rotation);
    void AddScale(const R32V3& Scale);

  private:

    const R32V3 mWorldRight = { 1.0F, 0.0F, 0.0F };
    const R32V3 mWorldUp = { 0.0F, 1.0F, 0.0F };
    const R32V3 mWorldFront = { 0.0F, 0.0F, 1.0F };

    R32V3 mLocalRight = { 1.0F, 0.0F, 0.0F };
    R32V3 mLocalUp = { 0.0F, 1.0F, 0.0F };
    R32V3 mLocalFront = { 0.0F, 0.0F, 1.0F };

    R32V3 mPosition = { 0.0F, 0.0F, 0.0F };
    R32V3 mRotation = { 0.0F, 0.0F, 0.0F };
    R32V3 mScale = { 1.0F, 1.0F, 1.0F };
  };
}