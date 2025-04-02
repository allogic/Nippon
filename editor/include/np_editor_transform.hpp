#if !defined(NP_EDITOR_TRANSFORM_HPP)
#  define NP_EDITOR_TRANSFORM_HPP

#  include <np_editor_constants.hpp>

class NpTransform {
public:
  NpTransform(NpTransform *Parent = nullptr);
  virtual ~NpTransform();

public:
  inline auto const &GetWorldPosition() const { return m_WorldPosition; }
  inline auto const &GetLocalRight() const { return m_LocalRight; }
  inline auto const &GetLocalUp() const { return m_LocalUp; }
  inline auto const &GetLocalFront() const { return m_LocalFront; }
  inline auto const &GetLocalLeft() const { return m_LocalLeft; }
  inline auto const &GetLocalDown() const { return m_LocalDown; }
  inline auto const &GetLocalBack() const { return m_LocalBack; }

private:
  NpTransform *m_Parent = nullptr;
  std::vector<NpTransform *> m_Children = {};

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
