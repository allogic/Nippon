#pragma once

#include <Ecs/Component.h>

namespace Nippon
{
	class CameraController : public Component
	{
	public:

		CameraController(Scene* Scene, Entity* Entity);
		virtual ~CameraController();

	public:

		inline const auto& GetKeyboardMovementSpeedNormal() const { return mKeyboardMovementSpeedNormal; }
		inline const auto& GetKeyboardMovementSpeedFast() const { return mKeyboardMovementSpeedFast; }
		inline const auto& GetMouseMovementSpeedNormal() const { return mMouseMovementSpeedNormal; }
		inline const auto& GetMouseMovementSpeedFast() const { return mMouseMovementSpeedFast; }
		inline const auto& GetMouseRotationSpeed() const { return mMouseRotationSpeed; }
		inline const auto& GetMouseDragDamping() const { return mMouseDragDamping; }

	public:

		inline void SetKeyboardMovementSpeedNormal(R32 Value) { mKeyboardMovementSpeedNormal = Value; }
		inline void SetKeyboardMovementSpeedFast(R32 Value) { mKeyboardMovementSpeedFast = Value; }
		inline void SetMouseMovementSpeedNormal(R32 Value) { mMouseMovementSpeedNormal = Value; }
		inline void SetMouseMovementSpeedFast(R32 Value) { mMouseMovementSpeedFast = Value; }
		inline void SetMouseRotationSpeed(R32 Value) { mMouseRotationSpeed = Value; }
		inline void SetMouseDragDamping(R32 Value) { mMouseDragDamping = Value; }

	private:

		R32 mKeyboardMovementSpeedNormal = 2.5F;
		R32 mKeyboardMovementSpeedFast = 25.0F;

		R32 mMouseMovementSpeedNormal = 0.025F;
		R32 mMouseMovementSpeedFast = 0.20F;

		R32 mMouseRotationSpeed = 5.5F;

		R32 mMouseDragDamping = 30.0F;
	};
}