#include <Ecs/Entity.h>

#include <Ecs/Components/Transform.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <Glm/gtc/matrix_transform.hpp>
#include <Glm/gtx/euler_angles.hpp>

namespace Nippon
{
	Transform::Transform(Scene* Scene, Entity* Entity) : Component(Scene, Entity)
	{
		ReComputeWorldPosition();
		ReComputeWorldRotation();
		ReComputeWorldScale();
	}

	Transform::~Transform()
	{

	}

	R32M4 Transform::GetModelMatrix()
	{
		R32M4 model = glm::identity<R32M4>();

		model = glm::translate(model, mCachedWorldPosition);
		model *= glm::mat4_cast(mCachedWorldRotation);
		model = glm::scale(model, mCachedWorldScale);

		return model;
	}

	void Transform::ReComputeWorldPosition()
	{
		Entity* entity = GetEntity();

		if (Entity* parent = entity->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				mCachedWorldPosition = transform->mCachedWorldPosition + transform->mCachedWorldRotation * mLocalPosition;
			}
			else
			{
				mCachedWorldPosition = mLocalRotation * mLocalPosition;
			}
		}
		else
		{
			mCachedWorldPosition = mLocalRotation * mLocalPosition;
		}

		for (auto const& child : entity->GetChildren())
		{
			if (Transform* transform = child->GetTransform())
			{
				transform->ReComputeWorldPosition();
			}
		}
	}

	void Transform::ReComputeWorldRotation()
	{
		Entity* entity = GetEntity();

		if (Entity* parent = entity->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				mCachedWorldRotation = mLocalRotation * transform->mCachedWorldRotation;
			}
			else
			{
				mCachedWorldRotation = mLocalRotation;
			}
		}
		else
		{
			mCachedWorldRotation = mLocalRotation;
		}

		for (auto const& child : entity->GetChildren())
		{
			if (Transform* transform = child->GetTransform())
			{
				transform->ReComputeWorldRotation();
			}
		}
	}

	void Transform::ReComputeWorldScale()
	{
		Entity* entity = GetEntity();

		if (Entity* parent = entity->GetParent())
		{
			if (Transform* transform = parent->GetTransform())
			{
				mCachedWorldScale = mLocalScale * transform->mCachedWorldScale;
			}
			else
			{
				mCachedWorldScale = mLocalScale;
			}
		}
		else
		{
			mCachedWorldScale = mLocalScale;
		}

		for (auto const& child : entity->GetChildren())
		{
			if (Transform* transform = child->GetTransform())
			{
				transform->ReComputeWorldPosition();
			}
		}
	}

	void Transform::SetPosition(R32V3 Position)
	{
		mLocalPosition = Position;

		ReComputeWorldPosition();
	}

	void Transform::SetRotation(R32V3 Rotation)
	{
		mLocalEulerAngles = glm::radians(Rotation);

		if (Entity* parent = GetEntity()->GetParent())
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

		ReComputeWorldRotation();
		ReComputeWorldPosition();
	}

	void Transform::SetScale(R32V3 Scale)
	{
		mLocalScale = Scale;

		ReComputeWorldScale();
	}

	void Transform::AddPosition(R32V3 Position)
	{
		mLocalPosition += Position;

		ReComputeWorldPosition();
	}

	void Transform::AddRotation(R32V3 Rotation)
	{
		mLocalEulerAngles += glm::radians(Rotation);

		if (Entity* parent = GetEntity()->GetParent())
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

		ReComputeWorldRotation();
		ReComputeWorldPosition();
	}

	void Transform::AddScale(R32V3 Scale)
	{
		mLocalScale += Scale;

		ReComputeWorldScale();
	}
}