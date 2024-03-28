#pragma once

#include <Ecs/Component.h>

namespace Nippon
{
	class Transform : public Component
	{
	public:

		Transform(Scene* Scene, Entity* Entity);
		virtual ~Transform();

	public:

		inline auto const& GetWorldRight() const { return mWorldRight; }
		inline auto const& GetWorldUp() const { return mWorldUp; }
		inline auto const& GetWorldFront() const { return mWorldFront; }

		inline auto const& GetLocalRight() const { return mLocalRight; }
		inline auto const& GetLocalUp() const { return mLocalUp; }
		inline auto const& GetLocalFront() const { return mLocalFront; }

	public:

		inline auto const& GetWorldPosition() const { return mCachedWorldPosition; }
		inline auto const& GetWorldRotation() const { return mCachedWorldRotation; }
		inline auto const& GetWorldScale() const { return mCachedWorldScale; }

		inline auto const GetLocalEulerAngles() const { return glm::degrees(mLocalEulerAngles); }

		inline auto const& GetLocalPosition() const { return mLocalPosition; }
		inline auto const& GetLocalRotation() const { return mLocalRotation; }
		inline auto const& GetLocalScale() const { return mLocalScale; }

	public:

		R32M4 GetModelMatrix();

	public:

		void ReComputeWorldPosition();
		void ReComputeWorldRotation();
		void ReComputeWorldScale();

	public:

		void SetPosition(R32V3 Position);
		void SetRotation(R32V3 Rotation);
		void SetScale(R32V3 Scale);

	public:

		void AddPosition(R32V3 Position);
		void AddRotation(R32V3 Rotation);
		void AddScale(R32V3 Scale);

	private:

		R32V3 const mWorldRight = { 1.0F, 0.0F, 0.0F };
		R32V3 const mWorldUp = { 0.0F, 1.0F, 0.0F };
		R32V3 const mWorldFront = { 0.0f, 0.0F, 1.0F };

		R32V3 mLocalRight = { 1.0F, 0.0F, 0.0F };
		R32V3 mLocalUp = { 0.0F, 1.0F, 0.0F };
		R32V3 mLocalFront = { 0.0F, 0.0F, 1.0F };

		R32V3 mLocalPosition = { 0.0F, 0.0F, 0.0F };
		R32V3 mLocalEulerAngles = { 0.0F, 0.0F, 0.0F };
		R32Q mLocalRotationX = { 1.0F, 0.0F, 0.0F, 0.0F };
		R32Q mLocalRotationY = { 1.0F, 0.0F, 0.0F, 0.0F };
		R32Q mLocalRotationZ = { 1.0F, 0.0F, 0.0F, 0.0F };
		R32Q mLocalRotation = { 1.0F, 0.0F, 0.0F, 0.0F };
		R32V3 mLocalScale = { 1.0F, 1.0F, 1.0F };

		R32V3 mCachedWorldPosition = {};
		R32Q mCachedWorldRotation = {};
		R32V3 mCachedWorldScale = {};
	};
}