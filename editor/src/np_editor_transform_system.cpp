#include <np_editor_pch.hpp>
#include <np_editor_transform_system.hpp>

void NpTransformSystem::Update(entt::registry &Registry) {
  auto View = Registry.view<NpTransformComponent>();

  for (auto Entity : View) {
    auto &Transform = View.get<NpTransformComponent>(Entity);

    Transform.ComputeWorldPosition();
    Transform.ComputeWorldRotation();
    Transform.ComputeWorldScale();
  }
}
