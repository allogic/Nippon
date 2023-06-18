#include <Editor/Event.h>

#include <Editor/Actors/Player.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Player::Player(std::string const& Name)
    : Actor{ Name }
    , mCamera{ AttachComponent<Camera>() }
  {

  }

  void Player::Update(R32 TimeDelta)
  {
    R32 keyboardMovementSpeed = (Event::KeyHeld(Event::eKeyCodeLeftShift)) ? mKeyboardMovementSpeedFast : mKeyboardMovementSpeedNormal;

    if (Event::KeyHeld(Event::eKeyCodeD)) GetTransform()->AddPosition(-GetTransform()->GetLocalRight() * keyboardMovementSpeed);
    if (Event::KeyHeld(Event::eKeyCodeA)) GetTransform()->AddPosition(GetTransform()->GetLocalRight() * keyboardMovementSpeed);

    if (Event::KeyHeld(Event::eKeyCodeE)) GetTransform()->AddPosition(GetTransform()->GetWorldUp() * keyboardMovementSpeed);
    if (Event::KeyHeld(Event::eKeyCodeQ)) GetTransform()->AddPosition(-GetTransform()->GetWorldUp() * keyboardMovementSpeed);

    if (Event::KeyHeld(Event::eKeyCodeW)) GetTransform()->AddPosition(GetTransform()->GetLocalFront() * keyboardMovementSpeed);
    if (Event::KeyHeld(Event::eKeyCodeS)) GetTransform()->AddPosition(-GetTransform()->GetLocalFront() * keyboardMovementSpeed);

    static R32V2 mousePositionStart;
    static R32V2 mousePositionDelta;

    if (Event::MouseDown(Event::eMouseCodeRight))
    {
      mousePositionStart = Event::GetMousePosition();
    }
    if (Event::MouseHeld(Event::eMouseCodeRight) && Event::MouseHeld(Event::eMouseCodeLeft))
    {
      mousePositionDelta = mousePositionStart - Event::GetMousePosition();

      R32V3 worldPosition = GetTransform()->GetPosition();

      R32 mouseMovementSpeed = (Event::KeyHeld(Event::eKeyCodeLeftShift)) ? mMouseMovementSpeedFast : mMouseMovementSpeedNormal;

      worldPosition += GetTransform()->GetLocalRight() * mousePositionDelta.x * mouseMovementSpeed;
      worldPosition += GetTransform()->GetWorldUp() * mousePositionDelta.y * mouseMovementSpeed;

      GetTransform()->SetPosition(worldPosition);
    }
    else if (Event::MouseHeld(Event::eMouseCodeRight))
    {
      mousePositionDelta = mousePositionStart - Event::GetMousePosition();

      R32V3 worldRotation = GetTransform()->GetRotation();

      worldRotation.x -= mousePositionDelta.y * mMouseRotationSpeed;
      worldRotation.y += mousePositionDelta.x * mMouseRotationSpeed;

      if (worldRotation.x < -90.0F) worldRotation.x = -90.0F;
      if (worldRotation.x > 90.0F) worldRotation.x = 90.0F;

      GetTransform()->SetRotation(worldRotation);
    }

    mousePositionStart -= mousePositionDelta * mMouseDragDamping;
  }
}