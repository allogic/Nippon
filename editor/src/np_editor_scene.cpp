#include <np_editor_pch.hpp>
#include <np_editor_renderer.hpp>
#include <np_editor_scene.hpp>
#include <np_editor_transform_system.hpp>

NpScene::NpScene() {
  CreatePlayer();

  m_Renderer.Create(0);
  m_Renderer.EnableDebug();
}
NpScene::~NpScene() {
  m_Renderer.Destroy();
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

  m_Renderer.DrawDebugLine({0.0F, 0.0F, 0.0F}, {100.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 1.0F});
  m_Renderer.DrawDebugLine({0.0F, 0.0F, 0.0F}, {0.0F, 100.0F, 0.0F}, {0.0F, 1.0F, 0.0F, 1.0F});
  m_Renderer.DrawDebugLine({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 100.0F}, {0.0F, 0.0F, 1.0F, 1.0F});
  m_Renderer.Draw(PlayerTransform, PlayerCamera);
}
void NpScene::CreateRenderer() { m_Renderer.Create(1); }
void NpScene::DestroyRenderer() { m_Renderer.Destroy(); }
