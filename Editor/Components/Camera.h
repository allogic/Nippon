#pragma once

#include <Common/Types.h>

#include <Editor/Component.h>

namespace ark
{
	class Scene;
	class Actor;

	class Camera : public Component
	{
	public:

		Camera(Scene* Scene, Actor* Actor);

	public:

		inline const auto& GetFov() const { return mFov; }
		inline const auto& GetNear() const { return mNear; }
		inline const auto& GetFar() const { return mFar; }

	public:

		inline void SetFov(R32 Fov) { mFov = Fov; }
		inline void SetNear(R32 Near) { mNear = Near; }
		inline void SetFar(R32 Far) { mFar = Far; }

	public:

		R32M4 GetProjectionMatrix();
		R32M4 GetViewMatrix();

	private:

		R32 mFov = 45.0F;
		R32 mNear = 0.1F;
		R32 mFar = 50000.0F;
	};
}
