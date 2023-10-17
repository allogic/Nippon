#include <Editor/Actor.h>

#include <Editor/Components/Transform.h>

#include <Vendor/GLM/gtc/matrix_transform.hpp>
#include <Vendor/GLM/gtx/euler_angles.hpp>

namespace ark
{
	Transform::Transform(Scene* Scene, Actor* Actor)
		: Component{ Scene, Actor }
	{

	}

	R32M4 Transform::GetModelMatrix() const
	{
		R32M4 model = glm::identity<R32M4>();

		R32V3 position = GetWorldPosition();
		R32Q rotation = GetWorldRotation();
		R32V3 scale = GetWorldScale();

		model = glm::translate(model, position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);

		return model;
	}

	R32V3 Transform::GetWorldPosition() const
	{
		if (Actor* parent = mActor->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				return transform->GetWorldPosition() + transform->GetWorldRotation() * mLocalPosition;
			}
			else
			{
				return mLocalRotation * mLocalPosition;
			}
		}
		else
		{
			return mLocalRotation * mLocalPosition;
		}
	}

	R32Q Transform::GetWorldRotation() const
	{
		if (Actor* parent = mActor->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				return mLocalRotation * transform->GetWorldRotation();
			}
			else
			{
				return mLocalRotation;
			}
		}
		else
		{
			return mLocalRotation;
		}
	}

	R32V3 Transform::GetWorldScale() const
	{
		if (Actor* parent = mActor->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				return mLocalScale * transform->GetWorldScale();
			}
			else
			{
				return mLocalScale;
			}
		}
		else
		{
			return mLocalScale;
		}
	}

	void Transform::SetLocalPosition(R32V3 Position)
	{
		mLocalPosition = Position;
	}

	void Transform::SetLocalRotation(R32V3 Rotation)
	{
		mLocalEulerAngles = glm::radians(Rotation);

		if (Actor* parent = mActor->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				mLocalRotationX = glm::angleAxis(mLocalEulerAngles.x, transform->GetLocalRight());
				mLocalRotationY = glm::angleAxis(mLocalEulerAngles.y, transform->GetLocalUp());
				mLocalRotationZ = glm::angleAxis(mLocalEulerAngles.z, transform->GetLocalFront());

				mLocalRotation = glm::normalize(mLocalRotationY * mLocalRotationX * mLocalRotationZ);

				mLocalRight = glm::normalize(mLocalRotation * transform->GetLocalRight());
				mLocalUp = glm::normalize(mLocalRotation * transform->GetLocalUp());
				mLocalFront = glm::normalize(mLocalRotation * transform->GetLocalFront());
			}
			else
			{
				mLocalRotationX = glm::angleAxis(mLocalEulerAngles.x, mWorldRight);
				mLocalRotationY = glm::angleAxis(mLocalEulerAngles.y, mWorldUp);
				mLocalRotationZ = glm::angleAxis(mLocalEulerAngles.z, mWorldFront);

				mLocalRotation = glm::normalize(mLocalRotationY * mLocalRotationX * mLocalRotationZ);

				mLocalRight = glm::normalize(mLocalRotation * mWorldRight);
				mLocalUp = glm::normalize(mLocalRotation * mWorldUp);
				mLocalFront = glm::normalize(mLocalRotation * mWorldFront);
			}
		}
		else
		{
			mLocalRotationX = glm::angleAxis(mLocalEulerAngles.x, mWorldRight);
			mLocalRotationY = glm::angleAxis(mLocalEulerAngles.y, mWorldUp);
			mLocalRotationZ = glm::angleAxis(mLocalEulerAngles.z, mWorldFront);

			mLocalRotation = glm::normalize(mLocalRotationY * mLocalRotationX * mLocalRotationZ);

			mLocalRight = glm::normalize(mLocalRotation * mWorldRight);
			mLocalUp = glm::normalize(mLocalRotation * mWorldUp);
			mLocalFront = glm::normalize(mLocalRotation * mWorldFront);
		}
	}

	void Transform::SetLocalScale(R32V3 Scale)
	{
		mLocalScale = Scale;
	}

	void Transform::AddLocalPosition(R32V3 Position)
	{
		mLocalPosition += Position;
	}

	void Transform::AddLocalRotation(R32V3 Rotation)
	{
		mLocalEulerAngles += glm::radians(Rotation);

		if (Actor* parent = mActor->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				mLocalRotationX = glm::angleAxis(mLocalEulerAngles.x, transform->GetLocalRight());
				mLocalRotationY = glm::angleAxis(mLocalEulerAngles.y, transform->GetLocalUp());
				mLocalRotationZ = glm::angleAxis(mLocalEulerAngles.z, transform->GetLocalFront());

				mLocalRotation = glm::normalize(mLocalRotationY * mLocalRotationX * mLocalRotationZ);

				mLocalRight = glm::normalize(mLocalRotation * transform->GetLocalRight());
				mLocalUp = glm::normalize(mLocalRotation * transform->GetLocalUp());
				mLocalFront = glm::normalize(mLocalRotation * transform->GetLocalFront());
			}
			else
			{
				mLocalRotationX = glm::angleAxis(mLocalEulerAngles.x, mWorldRight);
				mLocalRotationY = glm::angleAxis(mLocalEulerAngles.y, mWorldUp);
				mLocalRotationZ = glm::angleAxis(mLocalEulerAngles.z, mWorldFront);

				mLocalRotation = glm::normalize(mLocalRotationY * mLocalRotationX * mLocalRotationZ);

				mLocalRight = glm::normalize(mLocalRotation * mWorldRight);
				mLocalUp = glm::normalize(mLocalRotation * mWorldUp);
				mLocalFront = glm::normalize(mLocalRotation * mWorldFront);
			}
		}
		else
		{
			mLocalRotationX = glm::angleAxis(mLocalEulerAngles.x, mWorldRight);
			mLocalRotationY = glm::angleAxis(mLocalEulerAngles.y, mWorldUp);
			mLocalRotationZ = glm::angleAxis(mLocalEulerAngles.z, mWorldFront);

			mLocalRotation = glm::normalize(mLocalRotationY * mLocalRotationX * mLocalRotationZ);

			mLocalRight = glm::normalize(mLocalRotation * mWorldRight);
			mLocalUp = glm::normalize(mLocalRotation * mWorldUp);
			mLocalFront = glm::normalize(mLocalRotation * mWorldFront);
		}
	}

	void Transform::AddLocalScale(R32V3 Scale)
	{
		mLocalScale += Scale;
	}
}
