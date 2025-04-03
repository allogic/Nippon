#include <np_editor_pch.hpp>
#include <np_editor_transform_component.hpp>

NpTransformComponent::NpTransformComponent() {}
NpTransformComponent::NpTransformComponent(NpTransformComponent *Parent) : m_Parent(Parent) {}
NpTransformComponent::~NpTransformComponent() {}

void NpTransformComponent::SetPosition(glm::fvec3 const &Position) {
  m_LocalPosition = Position;

  ComputeWorldPosition();
}
void NpTransformComponent::SetRelativePosition(glm::fvec3 const &Position) {
  m_LocalPosition += Position;

  ComputeWorldPosition();
}
void NpTransformComponent::SetRotation(glm::fquat const &Rotation) {
  m_LocalRotation = glm::normalize(Rotation);

  glm::mat3 RotationMatrix = glm::mat3_cast(m_LocalRotation);

  m_LocalRight = RotationMatrix[0];
  m_LocalUp = RotationMatrix[1];
  m_LocalFront = RotationMatrix[2];
  m_LocalLeft = -m_LocalRight;
  m_LocalDown = -m_LocalUp;
  m_LocalBack = -m_LocalFront;

  ComputeWorldRotation();
  ComputeWorldPosition();
}
void NpTransformComponent::SetRelativeRotation(glm::fquat const &Rotation) {
  m_LocalRotation *= glm::normalize(Rotation);

  glm::mat3 RotationMatrix = glm::mat3_cast(m_LocalRotation);

  m_LocalRight = RotationMatrix[0];
  m_LocalUp = RotationMatrix[1];
  m_LocalFront = RotationMatrix[2];
  m_LocalLeft = -m_LocalRight;
  m_LocalDown = -m_LocalUp;
  m_LocalBack = -m_LocalFront;

  ComputeWorldRotation();
  ComputeWorldPosition();
}
void NpTransformComponent::SetScale(glm::fvec3 const &Scale) {
  m_LocalScale = Scale;

  ComputeWorldScale();
}
void NpTransformComponent::SetRelativeScale(glm::fvec3 const &Scale) {
  m_LocalScale += Scale;

  ComputeWorldScale();
}

void NpTransformComponent::ComputeWorldPosition() {
  if (m_Parent) {
    m_WorldPosition += m_LocalPosition * m_Parent->m_WorldRotation;
  } else {
    m_WorldPosition = m_LocalPosition;
  }

  for (auto const &Child : m_Children) {
    Child->ComputeWorldPosition();
  }
}
void NpTransformComponent::ComputeWorldRotation() {
  if (m_Parent) {
    m_WorldRotation = m_LocalRotation * m_Parent->m_WorldRotation;
  } else {
    m_WorldRotation = m_LocalRotation;
  }

  for (auto const &Child : m_Children) {
    Child->ComputeWorldRotation();
  }
}
void NpTransformComponent::ComputeWorldScale() {
  if (m_Parent) {
    m_WorldScale = m_LocalScale * m_Parent->m_WorldScale;
  } else {
    m_WorldScale = m_LocalScale;
  }

  for (auto const &Child : m_Children) {
    Child->ComputeWorldScale();
  }
}
