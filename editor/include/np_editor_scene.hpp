#if !defined(NP_EDITOR_SCENE_HPP)
#  define NP_EDITOR_SCENE_HPP

#  include <np_editor_camera_component.hpp>
#  include <np_editor_renderer.hpp>
#  include <np_editor_transform_component.hpp>

class NpScene {
public:
  NpScene();
  virtual ~NpScene();

  inline auto const &GetPlayerTransform() const { return m_Registry.get<NpTransformComponent>(m_Player); }
  inline auto const &GetPlayerCamera() const { return m_Registry.get<NpCameraComponent>(m_Player); }

  void Update();
  void CreateRenderer();
  void DestroyRenderer();

private:
  void CreatePlayer();

private:
  NpRenderer m_Renderer = {};

  entt::registry m_Registry = {};
  entt::entity m_Player = {};
};

#endif
