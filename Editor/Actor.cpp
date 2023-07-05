#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	Actor::Actor(Scene* Scene, const std::string& Name)
		: mScene{ Scene }
		, mName{ Name }
		, mTransform{ AttachComponent<Transform>() }
	{

	}

	Actor::~Actor()
	{
		for (auto& [hash, component] : mComponents)
		{
			delete component;
			component = nullptr;
		}
	}

	void Actor::MakeActiveRecursive(bool Active)
	{
		mActive = Active;

		for (Actor* child : mChildren)
		{
			child->MakeActiveRecursive(Active);
		}
	}

	void Actor::ComputeAxisAlignedBoundingBoxRecursive()
	{
		Renderable* renderable = GetComponent<Renderable>();

		if (renderable)
		{
			const auto& vertexBuffer = renderable->GetVertexBuffer();

			R32V3 min = mTransform->GetWorldRotation() * (vertexBuffer[0].Position * mTransform->GetWorldScale());
			R32V3 max = mTransform->GetWorldRotation() * (vertexBuffer[0].Position * mTransform->GetWorldScale());

			for (const auto& vertex : vertexBuffer)
			{
				R32V3 position = mTransform->GetWorldRotation() * (vertex.Position * mTransform->GetWorldScale());

				if (position.x < min.x) min.x = position.x;
				if (position.y < min.y) min.y = position.y;
				if (position.z < min.z) min.z = position.z;

				if (position.x > max.x) max.x = position.x;
				if (position.y > max.y) max.y = position.y;
				if (position.z > max.z) max.z = position.z;
			}

			R32V3 size = glm::abs(min - max);
			R32V3 center = min + size / 2.0F;

			mAABB = { min, max, size, center };
		}

		for (Actor* child : mChildren)
		{
			child->ComputeAxisAlignedBoundingBoxRecursive();
		}
	}
}