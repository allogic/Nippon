#include <Common/Macros.h>

#include <Editor/Actor.h>
#include <Editor/Event.h>
#include <Editor/Scene.h>
#include <Editor/Editor.h>
#include <Editor/InterfaceManager.h>

#include <Editor/Actors/Player.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/CameraController.h>
#include <Editor/Components/Transform.h>

#include <Editor/Interface/Outline.h>
#include <Editor/Interface/Viewport.h>

#include <Vendor/GLM/gtx/euler_angles.hpp>

namespace ark
{
	Player::Player(Scene* Scene, U32 Id, const std::string& Name)
		: Actor{ Scene, Id, Name }
		, mCamera{ AttachComponent<Camera>() }
		, mCameraController{ AttachComponent<CameraController>() }
	{

	}

	void Player::Update()
	{
		HandlePosition();
		HandleRotation();
		HandleActorFocus();
	}

	void Player::HandlePosition()
	{
		if (Viewport* viewport = mScene->GetViewport())
		{
			if (viewport->IsFocused())
			{
				R32 keyboardMovementSpeed = (Event::KeyHeld(eKeyCodeLeftShift)) ? mCameraController->GetKeyboardMovementSpeedFast() : mCameraController->GetKeyboardMovementSpeedNormal();

				if (Event::KeyHeld(eKeyCodeD)) GetTransform()->AddLocalPosition(GetTransform()->GetLocalRight() * keyboardMovementSpeed * gTimeDelta);
				if (Event::KeyHeld(eKeyCodeA)) GetTransform()->AddLocalPosition(-GetTransform()->GetLocalRight() * keyboardMovementSpeed * gTimeDelta);

				if (Event::KeyHeld(eKeyCodeE)) GetTransform()->AddLocalPosition(GetTransform()->GetWorldUp() * keyboardMovementSpeed * gTimeDelta);
				if (Event::KeyHeld(eKeyCodeQ)) GetTransform()->AddLocalPosition(-GetTransform()->GetWorldUp() * keyboardMovementSpeed * gTimeDelta);

				if (Event::KeyHeld(eKeyCodeW)) GetTransform()->AddLocalPosition(-GetTransform()->GetLocalFront() * keyboardMovementSpeed * gTimeDelta);
				if (Event::KeyHeld(eKeyCodeS)) GetTransform()->AddLocalPosition(GetTransform()->GetLocalFront() * keyboardMovementSpeed * gTimeDelta);
			}
		}
	}

	void Player::HandleRotation()
	{
		if (Viewport* viewport = mScene->GetViewport())
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

					positionOffset += GetTransform()->GetLocalRight() * mousePositionDelta.x * mouseMovementSpeed * gTimeDelta;
					positionOffset -= GetTransform()->GetWorldUp() * mousePositionDelta.y * mouseMovementSpeed * gTimeDelta;

					GetTransform()->AddLocalPosition(positionOffset);
				}
				else if (Event::MouseHeld(eMouseCodeRight))
				{
					mousePositionDelta = mousePositionStart - Event::GetMousePosition();

					R32V3 eulerAngles = GetTransform()->GetLocalEulerAngles();

					eulerAngles.x += mousePositionDelta.y * mCameraController->GetMouseRotationSpeed() * gTimeDelta;
					eulerAngles.y += mousePositionDelta.x * mCameraController->GetMouseRotationSpeed() * gTimeDelta;

					if (eulerAngles.x <= -90.0F) eulerAngles.x = -90.0F;
					if (eulerAngles.x >= 90.0F) eulerAngles.x = 90.0F;

					GetTransform()->SetLocalRotation(eulerAngles);
				}

				mousePositionStart -= mousePositionDelta * mCameraController->GetMouseDragDamping() * gTimeDelta;
			}
		}
	}

	void Player::HandleActorFocus()
	{
		if (Viewport* viewport = mScene->GetViewport())
		{
			if (viewport->IsFocused())
			{
				if (Event::KeyDown(eKeyCodeF))
				{
					if (Actor* actor = InterfaceManager::GetOutline()->GetSelectedActor())
					{
						GetTransform()->SetLocalPosition(R32V3{ 0.0F, 0.0F, 0.0F }); // TODO
					}
				}
			}
		}
	}
}
