#include <Editor/Actor.h>

#include <Editor/Components/Transform.h>

#include <Vendor/GLM/gtc/matrix_transform.hpp>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Transform::Transform(Actor* Actor)
    : Component{ Actor }
  {

  }

  R32M4 Transform::GetModelMatrix() const
  {
    R32M4 model = glm::identity<R32M4>();

    model = glm::translate(model, mPosition);
    model = glm::rotate(model, mRotation.x, mWorldRight);
    model = glm::rotate(model, mRotation.y, mWorldUp);
    model = glm::rotate(model, mRotation.z, mWorldFront);
    model = glm::scale(model, mScale);

    return model;
  }

  R32V3 Transform::GetWorldPosition() const
  {
    if (mActor->GetParent())
    {
      return mActor->GetParent()->GetTransform()->GetWorldPosition() + mPosition;
    }
    else
    {
      return mPosition;
    }
  }

  R32V3 Transform::GetWorldRotation() const
  {
    return glm::degrees(mRotation);
  }

  R32Q Transform::GetWorldQuaternion() const
  {
    return mRotation;
  }

  R32V3 Transform::GetWorldScale() const
  {
    return mScale;
  }

  void Transform::SetWorldPosition(const R32V3& Position)
  {
    mPosition = Position;
  }

  void Transform::SetWorldRotation(const R32V3& Rotation)
  {
    mRotation = glm::radians(Rotation);
  }

  void Transform::SetWorldScale(const R32V3& Scale)
  {
    mScale = Scale;
  }

  void Transform::SetLocalPosition(const R32V3& Position)
  {
    mPosition = Position;
  }

  void Transform::SetLocalRotation(const R32V3& Rotation)
  {
    mRotation = glm::radians(Rotation);

    R32Q q = mRotation;
    mLocalRight = q * mWorldRight;
    mLocalUp = q * mWorldUp;
    mLocalFront = q * mWorldFront;

    for (auto child : mActor->GetChildren())
    {
      child->GetTransform()->SetLocalRotation(GetRotation());
    }
  }

  void Transform::SetLocalScale(const R32V3& Scale)
  {
    mScale = Scale;

    for (auto child : mActor->GetChildren())
    {
      child->GetTransform()->SetLocalScale(GetScale());
    }
  }

  void Transform::AddWorldPosition(const R32V3& Position)
  {
    mPosition += Position;
  }

  void Transform::AddWorldRotation(const R32V3& Rotation)
  {
    mRotation += glm::radians(Rotation);
  }

  void Transform::AddWorldScale(const R32V3& Scale)
  {
    mScale += Scale;
  }

  void Transform::AddLocalPosition(const R32V3& Position)
  {
    mPosition += Position;
  }

  void Transform::AddLocalRotation(const R32V3& Rotation)
  {
    mRotation += glm::radians(Rotation);

    R32Q q = mRotation;
    mLocalRight = q * mWorldRight;
    mLocalUp = q * mWorldUp;
    mLocalFront = q * mWorldFront;

    for (auto child : mActor->GetChildren())
    {
      child->GetTransform()->SetLocalRotation(GetRotation());
    }
  }

  void Transform::AddLocalScale(const R32V3& Scale)
  {
    mScale += Scale;

    for (auto child : mActor->GetChildren())
    {
      child->GetTransform()->SetLocalScale(GetScale());
    }
  }

}
