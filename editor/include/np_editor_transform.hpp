#if !defined(NP_EDITOR_TRANSFORM_HPP)
#  define NP_EDITOR_TRANSFORM_HPP

class NpTransform {
public:
  NpTransform();
  NpTransform(NpTransform *Parent);
  virtual ~NpTransform();

public:
  inline auto const &GetWorldPosition() const { return m_WorldPosition; }

private:
  NpTransform *m_Parent = nullptr;
  std::vector<NpTransform *> m_Children = {};

  glm::fvec3 m_LocalPosition = {0.0F, 0.0F, 0.0F};
  glm::fquat m_LocalRotation = {1.0F, 0.0F, 0.0F, 0.0F};
  glm::fvec3 m_LocalScale = {1.0F, 1.0F, 1.0F};
  glm::fvec3 m_WorldPosition = {0.0F, 0.0F, 0.0F};
  glm::fquat m_WorldRotation = {1.0F, 0.0F, 0.0F, 0.0F};
  glm::fvec3 m_WorldScale = {1.0F, 1.0F, 1.0F};
};

#endif
