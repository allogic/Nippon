#include <np_editor_pch.hpp>
#include <np_editor_renderer.hpp>
#include <np_editor_scene.hpp>
#include <np_editor_transform_system.hpp>

NpScene::NpScene() {
  CreatePlayer();
}
NpScene::~NpScene() {
}

void NpScene::CreatePlayer() {
  m_Player = m_Registry.create();

  NpTransformComponent &Transform = m_Registry.emplace<NpTransformComponent>(m_Player);
  NpCameraComponent &Camera = m_Registry.emplace<NpCameraComponent>(m_Player);

  Transform.SetPosition({0.0F, 0.0F, -10.0F});
}

void NpScene::Update() {
  NpTransformSystem::Update(m_Registry);

  NpTransformComponent const &PlayerTransform = GetPlayerTransform();
  NpCameraComponent const &PlayerCamera = GetPlayerCamera();

  g_Renderer.DrawDebugLine({0.0F, 0.0F, 0.0F}, {100.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 1.0F});
  g_Renderer.DrawDebugLine({0.0F, 0.0F, 0.0F}, {0.0F, 100.0F, 0.0F}, {0.0F, 1.0F, 0.0F, 1.0F});
  g_Renderer.DrawDebugLine({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 100.0F}, {0.0F, 0.0F, 1.0F, 1.0F});
  g_Renderer.Draw(PlayerTransform, PlayerCamera);
}
