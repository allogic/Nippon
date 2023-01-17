#pragma once

#include <Common/Types.h>

#include <Editor/Component.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Actor;

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

    inline const auto& GetPosition() const { return mPosition; }
    inline auto GetRotation() const { return glm::degrees(mRotation); }
    inline auto GetQuaternion() const { return R32Q{ mRotation }; }
    inline const auto& GetScale() const { return mScale; }

  public:

    R32M4 GetModelMatrix() const;

  public:

    R32V3 GetWorldPosition() const;
    R32V3 GetWorldRotation() const;
    R32Q GetWorldQuaternion() const;
    R32V3 GetWorldScale() const;

  public:

    void SetWorldPosition(const R32V3& Position);
    void SetWorldRotation(const R32V3& Rotation);
    void SetWorldScale(const R32V3& Scale);

    void SetLocalPosition(const R32V3& Position);
    void SetLocalRotation(const R32V3& Rotation);
    void SetLocalScale(const R32V3& Scale);

  public:

    void AddWorldPosition(const R32V3& Position);
    void AddWorldRotation(const R32V3& Rotation);
    void AddWorldScale(const R32V3& Scale);

    void AddLocalPosition(const R32V3& Position);
    void AddLocalRotation(const R32V3& Rotation);
    void AddLocalScale(const R32V3& Scale);

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