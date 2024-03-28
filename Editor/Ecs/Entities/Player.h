#pragma once

#include <Ecs/Entity.h>

namespace Nippon
{
	class Player : public Entity
	{
	public:

		Player(Scene* Scene, U32 UniqueId, std::string const& Name);
		virtual ~Player();

	public:

		void Update() override;

	private:

		void HandlePosition();
		void HandleRotation();
		void HandleEntityFocus();

	private:

		Camera* mCamera;
		CameraController* mCameraController;
	};
}