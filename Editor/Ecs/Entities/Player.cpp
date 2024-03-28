#include <Editor.h>
#include <Event.h>

#include <Ecs/Components/Camera.h>
#include <Ecs/Components/CameraController.h>
#include <Ecs/Components/Transform.h>

#include <Ecs/Entities/Player.h>

#include <Interface/Viewport.h>
#include <Interface/Outline.h>

#include <Scene/Scene.h>

namespace Nippon
{
	Player::Player(Scene* Scene, U32 UniqueId, std::string const& Name) : Entity(Scene, UniqueId, Name)
	{
		mCamera = AttachComponent<Camera>();
		mCameraController = AttachComponent<CameraController>();
	}

	Player::~Player()
	{

	}

	void Player::Update()
	{
		HandlePosition();
		HandleRotation();
		HandleEntityFocus();
	}

	void Player::HandlePosition()
	{
		if (Viewport* viewport = GetScene()->GetViewport())
		{
			if (viewport->IsFocused())
			{
				R32 keyboardMovementSpeed = (Event::KeyHeld(eKeyCodeLeftShift)) ? mCameraController->GetKeyboardMovementSpeedFast() : mCameraController->GetKeyboardMovementSpeedNormal();

				if (Event::KeyHeld(eKeyCodeD)) GetTransform()->AddPosition(GetTransform()->GetLocalRight() * keyboardMovementSpeed * gDeltaTime);
				if (Event::KeyHeld(eKeyCodeA)) GetTransform()->AddPosition(-GetTransform()->GetLocalRight() * keyboardMovementSpeed * gDeltaTime);

				if (Event::KeyHeld(eKeyCodeE)) GetTransform()->AddPosition(GetTransform()->GetWorldUp() * keyboardMovementSpeed * gDeltaTime);
				if (Event::KeyHeld(eKeyCodeQ)) GetTransform()->AddPosition(-GetTransform()->GetWorldUp() * keyboardMovementSpeed * gDeltaTime);

				if (Event::KeyHeld(eKeyCodeW)) GetTransform()->AddPosition(-GetTransform()->GetLocalFront() * keyboardMovementSpeed * gDeltaTime);
				if (Event::KeyHeld(eKeyCodeS)) GetTransform()->AddPosition(GetTransform()->GetLocalFront() * keyboardMovementSpeed * gDeltaTime);
			}
		}
	}

	void Player::HandleRotation()
	{
		if (Viewport* viewport = GetScene()->GetViewport())
		{
			if (viewport->IsFocused())
			{
				static R32V2 mousePositionStart;
				static R32V2 mousePositionDelta;

				if (Event::MouseDown(eMouseCodeRight))
				{
					mousePositionStart = Event::GetMousePosition();
				}
				if (Event::MouseHeld(eMouseCodeRight) && Event::MouseHeld(eMouseCodeLeft))
				{
					mousePositionDelta = mousePositionStart - Event::GetMousePosition();

					R32V3 positionOffset = {};

					R32 mouseMovementSpeed = (Event::KeyHeld(eKeyCodeLeftShift)) ? mCameraController->GetMouseMovementSpeedFast() : mCameraController->GetMouseMovementSpeedNormal();

					positionOffset += GetTransform()->GetLocalRight() * mousePositionDelta.x * mouseMovementSpeed * gDeltaTime;
					positionOffset -= GetTransform()->GetWorldUp() * mousePositionDelta.y * mouseMovementSpeed * gDeltaTime;

					GetTransform()->AddPosition(positionOffset);
				}
				else if (Event::MouseHeld(eMouseCodeRight))
				{
					mousePositionDelta = mousePositionStart - Event::GetMousePosition();

					R32V3 eulerAngles = GetTransform()->GetLocalEulerAngles();

					eulerAngles.x += mousePositionDelta.y * mCameraController->GetMouseRotationSpeed() * gDeltaTime;
					eulerAngles.y += mousePositionDelta.x * mCameraController->GetMouseRotationSpeed() * gDeltaTime;

					if (eulerAngles.x <= -90.0F) eulerAngles.x = -90.0F;
					if (eulerAngles.x >= 90.0F) eulerAngles.x = 90.0F;

					GetTransform()->SetRotation(eulerAngles);
				}

				mousePositionStart -= mousePositionDelta * mCameraController->GetMouseDragDamping() * gDeltaTime;
			}
		}
	}

	void Player::HandleEntityFocus()
	{
		if (Viewport* viewport = GetScene()->GetViewport())
		{
			if (viewport->IsFocused())
			{
				if (Event::KeyDown(eKeyCodeF))
				{
					if (Entity* entity = Outline::GetSelectedEntity())
					{
						GetTransform()->SetPosition(R32V3{ 0.0F, 0.0F, 0.0F }); // TODO
					}
				}
			}
		}
	}
}