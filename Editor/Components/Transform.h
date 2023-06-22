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

    Transform(Scene* Scene, Actor* Actor);

  public:

    inline auto const& GetWorldRight() const { return mWorldRight; }
    inline auto const& GetWorldUp() const { return mWorldUp; }
    inline auto const& GetWorldFront() const { return mWorldFront; }

    inline auto const& GetLocalRight() const { return mLocalRight; }
    inline auto const& GetLocalUp() const { return mLocalUp; }
    inline auto const& GetLocalFront() const { return mLocalFront; }

  public:

    inline const auto GetLocalEulerAngles() const { return glm::degrees(mLocalEulerAngles); }

    inline const auto& GetLocalPosition() const { return mLocalPosition; }
    inline const auto& GetLocalRotation() const { return mLocalRotation; }
    inline const auto& GetLocalScale() const { return mLocalScale; }

  public:

    R32M4 GetModelMatrix() const;

  public:

    R32V3 GetWorldPosition() const;
    R32Q GetWorldRotation() const;
    R32V3 GetWorldScale() const;

  public:

    void SetLocalPosition(R32V3 Position);
    void SetLocalRotation(R32V3 Rotation);
    void SetLocalScale(R32V3 Scale);

  public:

    void AddLocalPosition(R32V3 Position);
    void AddLocalRotation(R32V3 Rotation);
    void AddLocalScale(R32V3 Scale);

  private:

    const R32V3 mWorldRight = { 1.0F, 0.0F, 0.0F };
    const R32V3 mWorldUp = { 0.0F, 1.0F, 0.0F };
    const R32V3 mWorldFront = { 0.0f, 0.0F, 1.0F };

    R32V3 mLocalRight = { 1.0F, 0.0F, 0.0F };
    R32V3 mLocalUp = { 0.0F, 1.0F, 0.0F };
    R32V3 mLocalFront = { 0.0F, 0.0F, 1.0F };

    R32V3 mLocalPosition = { 0.0F, 0.0F, 0.0F };
    R32V3 mLocalEulerAngles = { 0.0F, 0.0F, 0.0F };
    R32Q mLocalRotationX = { 1.0F, 0.0F, 0.0F, 0.0F };
    R32Q mLocalRotationY = { 1.0F, 0.0F, 0.0F, 0.0F };
    R32Q mLocalRotationZ = { 1.0F, 0.0F, 0.0F, 0.0F };
    R32Q mLocalRotation = { 1.0F, 0.0F, 0.0F, 0.0F };
    R32V3 mLocalScale = { 1.0F, 1.0F, 1.0F };
  };
}