#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Camera.h>

#include <Vendor/GLM/gtc/matrix_transform.hpp>

namespace ark
{
	Camera::Camera(Scene* Scene, Actor* Actor)
		: Component{ Scene, Actor }
	{

	}

	R32M4 Camera::GetProjectionMatrix()
	{
		R32 fov = glm::radians(mFov);
		R32 aspectRatio = ((R32)mScene->GetWidth() / mScene->GetHeight());

		return glm::perspective(fov, aspectRatio, mNear, mFar);
	}

	R32M4 Camera::GetViewMatrix()
	{
		R32V3 position = mActor->GetTransform()->GetWorldPosition();
		R32V3 up = mActor->GetTransform()->GetLocalUp();
		R32V3 front = mActor->GetTransform()->GetLocalFront();

		return glm::lookAtLH(position, position + front, up);
	}
}
