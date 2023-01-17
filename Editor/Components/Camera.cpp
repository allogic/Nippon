#include <Editor/Actor.h>
#include <Editor/Window.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Camera.h>

#include <Vendor/GLM/gtc/matrix_transform.hpp>

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
    return glm::perspective(glm::radians(mFov), Window::GetAspectRatio(), mNear, mFar);
  }

  R32M4 Camera::GetViewMatrix()
  {
    R32V3 position = mActor->GetTransform()->GetWorldPosition();
    R32V3 up = mActor->GetTransform()->GetLocalUp();
    R32V3 front = mActor->GetTransform()->GetLocalFront();

    return glm::lookAt(position, position + front, up);
  }
}