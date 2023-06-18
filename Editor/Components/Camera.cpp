#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Camera.h>

#include <Vendor/GLM/gtc/matrix_transform.hpp>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Camera::Camera(Actor* Actor)
    : Component{ Actor }
  {

  }

  R32M4 Camera::GetProjectionMatrix()
  {
    R32 fov = glm::radians(mFov);
    R32 aspectRatio = (gScene) ? ((R32)gScene->GetWidth() / gScene->GetHeight()) : 1.0F;

    return glm::perspective(fov, aspectRatio, mNear, mFar);
  }

  R32M4 Camera::GetViewMatrix()
  {
    R32V3 position = mActor->GetTransform()->GetPosition();
    R32V3 up = mActor->GetTransform()->GetLocalUp();
    R32V3 front = mActor->GetTransform()->GetLocalFront();

    return glm::lookAt(position, position + front, up);
  }
}