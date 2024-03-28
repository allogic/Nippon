#include <Ecs/Entity.h>

#include <Ecs/Components/Camera.h>
#include <Ecs/Components/Transform.h>

#include <Scene/Scene.h>

namespace Nippon
{
	Camera::Camera(Scene* Scene, Entity* Entity) : Component(Scene, Entity)
	{

	}

	Camera::~Camera()
	{

	}

	R32M4 Camera::GetProjectionMatrix()
	{
		Scene* scene = GetScene();

		R32 fov = glm::radians(mFov);
		R32 aspectRatio = scene->GetAspectRatio();

		return glm::perspective(fov, aspectRatio, mNear, mFar);
	}

	R32M4 Camera::GetViewMatrix()
	{
		Entity* entity = GetEntity();
		Transform* transform = entity->GetTransform();

		R32V3 const& position = transform->GetWorldPosition();
		R32V3 const& up = transform->GetLocalUp();
		R32V3 const& front = transform->GetLocalFront();

		return glm::lookAtLH(position, position + front, up);
	}
}