#pragma once

#include <string>

#include <Common/Types.h>

#include <Editor/Actor.h>

namespace ark
{
	class Camera;
	class CameraController;

	class Player : public Actor
	{
	public:

		Player(Scene* Scene, U32 Id, const std::string& Name);

	public:

		void Update() override;

	private:

		void HandlePosition();
		void HandleRotation();
		void HandleActorFocus();

	private:

		Camera* mCamera;
		CameraController* mCameraController;
	};
}
