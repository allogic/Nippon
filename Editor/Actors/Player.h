#pragma once

#include <string>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class Player : public Actor
	{
	public:

		Player(Scene* Scene, std::string const& Name);

	public:

		void Update(R32 TimeDelta) override;

	private:

		Camera* mCamera;

		R32 mKeyboardMovementSpeedNormal = 0.1F;
		R32 mKeyboardMovementSpeedFast = 1.0F;

		R32 mMouseMovementSpeedNormal = 0.005F;
		R32 mMouseMovementSpeedFast = 0.2F;

		R32 mMouseRotationSpeed = 0.095F;

		R32 mMouseDragDamping = 0.5F;
	};
}