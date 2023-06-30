#include <Editor/Event.h>
#include <Editor/Scene.h>

#include <Editor/Actors/Player.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Editor/Interface/Viewport.h>

#include <Vendor/GLM/gtx/euler_angles.hpp>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Player::Player(Scene* Scene, std::string const& Name)
    : Actor{ Scene, Name }
    , mCamera{ AttachComponent<Camera>() }
  {

  }

  void Player::Update(R32 TimeDelta)
  {
    Viewport* viewport = mScene->GetViewport();

    if (viewport)
    {
      if (viewport->IsFocused())
      {
        R32 keyboardMovementSpeed = (Event::KeyHeld(Event::eKeyCodeLeftShift)) ? mKeyboardMovementSpeedFast : mKeyboardMovementSpeedNormal;

        if (Event::KeyHeld(Event::eKeyCodeD)) GetTransform()->AddLocalPosition(GetTransform()->GetLocalRight() * keyboardMovementSpeed);
        if (Event::KeyHeld(Event::eKeyCodeA)) GetTransform()->AddLocalPosition(-GetTransform()->GetLocalRight() * keyboardMovementSpeed);

        if (Event::KeyHeld(Event::eKeyCodeE)) GetTransform()->AddLocalPosition(GetTransform()->GetWorldUp() * keyboardMovementSpeed);
        if (Event::KeyHeld(Event::eKeyCodeQ)) GetTransform()->AddLocalPosition(-GetTransform()->GetWorldUp() * keyboardMovementSpeed);

        if (Event::KeyHeld(Event::eKeyCodeW)) GetTransform()->AddLocalPosition(-GetTransform()->GetLocalFront() * keyboardMovementSpeed);
        if (Event::KeyHeld(Event::eKeyCodeS)) GetTransform()->AddLocalPosition(GetTransform()->GetLocalFront() * keyboardMovementSpeed);

        static R32V2 mousePositionStart;
        static R32V2 mousePositionDelta;

        if (Event::MouseDown(Event::eMouseCodeRight))
        {
          mousePositionStart = Event::GetMousePosition();
        }
        if (Event::MouseHeld(Event::eMouseCodeRight) && Event::MouseHeld(Event::eMouseCodeLeft))
        {
          mousePositionDelta = mousePositionStart - Event::GetMousePosition();

          R32V3 positionOffset = {};

          R32 mouseMovementSpeed = (Event::KeyHeld(Event::eKeyCodeLeftShift)) ? mMouseMovementSpeedFast : mMouseMovementSpeedNormal;

          positionOffset += GetTransform()->GetLocalRight() * mousePositionDelta.x * mouseMovementSpeed;
          positionOffset -= GetTransform()->GetWorldUp() * mousePositionDelta.y * mouseMovementSpeed;

          GetTransform()->AddLocalPosition(positionOffset);
        }
        else if (Event::MouseHeld(Event::eMouseCodeRight))
        {
          mousePositionDelta = mousePositionStart - Event::GetMousePosition();

          R32V3 eulerAngles = GetTransform()->GetLocalEulerAngles();

          eulerAngles.x += mousePositionDelta.y * mMouseRotationSpeed;
          eulerAngles.y += mousePositionDelta.x * mMouseRotationSpeed;

          if (eulerAngles.x <= -90.0F) eulerAngles.x = -90.0F;
          if (eulerAngles.x >= 90.0F) eulerAngles.x = 90.0F;

          GetTransform()->SetLocalRotation(eulerAngles);
        }

        mousePositionStart -= mousePositionDelta * mMouseDragDamping;
      }
    }
  }
}