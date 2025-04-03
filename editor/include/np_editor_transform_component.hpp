#if !defined(NP_EDITOR_TRANSFORM_COMPONENT_HPP)
#  define NP_EDITOR_TRANSFORM_COMPONENT_HPP

#  include <np_editor_constants.hpp>

class NpTransformComponent {
public:
  NpTransformComponent();
  NpTransformComponent(NpTransformComponent *Parent);
  virtual ~NpTransformComponent();

public:
  inline auto const &GetParent() const { return m_Parent; }
  inline auto const &GetChildren() const { return m_Children; }
  inline auto const &GetWorldPosition() const { return m_WorldPosition; }
  inline auto const &GetLocalRight() const { return m_LocalRight; }
  inline auto const &GetLocalUp() const { return m_LocalUp; }
  inline auto const &GetLocalFront() const { return m_LocalFront; }
  inline auto const &GetLocalLeft() const { return m_LocalLeft; }
  inline auto const &GetLocalDown() const { return m_LocalDown; }
  inline auto const &GetLocalBack() const { return m_LocalBack; }

  void SetPosition(glm::fvec3 const &Position);
  void SetRelativePosition(glm::fvec3 const &Position);
  void SetRotation(glm::fquat const &Rotation);
  void SetRelativeRotation(glm::fquat const &Rotation);
  void SetScale(glm::fvec3 const &Scale);
  void SetRelativeScale(glm::fvec3 const &Scale);

  void ComputeWorldPosition();
  void ComputeWorldRotation();
  void ComputeWorldScale();

private:
  NpTransformComponent *m_Parent = nullptr;
  std::vector<NpTransformComponent *> m_Children = {};

  glm::fvec3 m_LocalPosition = {0.0F, 0.0F, 0.0F};
  glm::fquat m_LocalRotation = {1.0F, 0.0F, 0.0F, 0.0F};
  glm::fvec3 m_LocalScale = {1.0F, 1.0F, 1.0F};

  glm::fvec3 m_WorldPosition = {0.0F, 0.0F, 0.0F};
  glm::fquat m_WorldRotation = {1.0F, 0.0F, 0.0F, 0.0F};
  glm::fvec3 m_WorldScale = {1.0F, 1.0F, 1.0F};

  glm::fvec3 m_LocalRight = g_WorldRight;
  glm::fvec3 m_LocalUp = g_WorldUp;
  glm::fvec3 m_LocalFront = g_WorldFront;

  glm::fvec3 m_LocalLeft = g_WorldLeft;
  glm::fvec3 m_LocalDown = g_WorldDown;
  glm::fvec3 m_LocalBack = g_WorldBack;
};

#endif
