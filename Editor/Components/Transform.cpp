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

  R32V3 Transform::GetPosition() const
  {
    Actor* parent = mActor->GetParent();

    if (parent)
    {
      return parent->GetTransform()->GetPosition() + mPosition;
    }
    else
    {
      return mPosition;
    }
  }

  R32V3 Transform::GetRotation() const
  {
    return glm::degrees(mRotation);
  }

  R32Q Transform::GetQuaternion() const
  {
    return mRotation;
  }

  R32V3 Transform::GetScale() const
  {
    return mScale;
  }

  void Transform::SetPosition(const R32V3& Position)
  {
    mPosition = Position;
  }

  void Transform::SetRotation(const R32V3& Rotation)
  {
    mRotation = glm::radians(Rotation);

    R32Q q = mRotation;

    mLocalRight = q * mWorldRight;
    mLocalUp = q * mWorldUp;
    mLocalFront = q * mWorldFront;
  }

  void Transform::SetScale(const R32V3& Scale)
  {
    mScale = Scale;
  }

  void Transform::AddPosition(const R32V3& Position)
  {
    mPosition += Position;
  }

  void Transform::AddRotation(const R32V3& Rotation)
  {
    mRotation += glm::radians(Rotation);
  }

  void Transform::AddScale(const R32V3& Scale)
  {
    mScale += Scale;
  }
}