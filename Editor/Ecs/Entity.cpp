#include <Common/Memory.h>

#include <Common/VertexTypes.h>

#include <Editor/Ecs/Entity.h>
#include <Editor/Ecs/Component.h>

#include <Editor/Ecs/Components/Transform.h>
#include <Editor/Ecs/Components/Renderable.h>

#include <Editor/OpenGl/StaticMesh.h>

#include <Editor/Scene/Scene.h>

namespace Nippon
{
	Entity::Entity(Scene* Scene, U32 UniqueId, std::string const& Name)
	{
		mScene = Scene;
		mUniqueId = UniqueId;
		mName = Name;

		mTransform = AttachComponent<Transform>();
	}

	Entity::~Entity()
	{
		for (auto const& [hash, component] : mComponents)
		{
			component->~Component();

			Memory::Free(component);
		}
	}

	void Entity::SetShouldBeDestroyed()
	{
		mShouldBeDestroyed = true;

		mScene->SetDirty();
	}

	void Entity::SetShouldBeDestroyedUp()
	{
		mShouldBeDestroyed = true;

		mScene->SetDirty();

		if (mParent)
		{
			mParent->SetShouldBeDestroyedUp();
		}
	}

	void Entity::SetShouldBeDestroyedDown()
	{
		mShouldBeDestroyed = true;

		mScene->SetDirty();

		for (Entity* child : mChildren)
		{
			child->SetShouldBeDestroyedDown();
		}
	}

	void Entity::SetActiveUp(bool Value)
	{
		mActive = Value;

		if (mParent)
		{
			mParent->SetActiveUp(Value);
		}
	}

	void Entity::SetActiveDown(bool Value)
	{
		mActive = Value;

		for (Entity* child : mChildren)
		{
			child->SetActiveDown(Value);
		}
	}

	void Entity::SetOpenedUp(bool Value)
	{
		mOpened = Value;

		if (mParent)
		{
			mParent->SetOpenedUp(Value);
		}
	}

	void Entity::SetOpenedDown(bool Value)
	{
		mOpened = Value;

		for (Entity* child : mChildren)
		{
			child->SetOpenedDown(Value);
		}
	}

	void Entity::Update()
	{
		
	}

	void Entity::AddChild(Entity* Entity)
	{
		mChildren.emplace_back(Entity);
	}

	void Entity::RemoveChild(Entity* Entity)
	{
		auto const& findIt = std::find(mChildren.begin(), mChildren.end(), Entity);

		if (findIt != mChildren.end())
		{
			mChildren.erase(findIt);
		}
	}

	void Entity::ComputeAABB()
	{
		Renderable* renderable = GetComponent<Renderable>();

		if (renderable)
		{
			StaticMesh* staticMesh = renderable->GetStaticMesh();

			std::vector<DefaultVertex> vertices = {};

			U64 vertexBufferCount = staticMesh->GetVertexBufferCount();

			vertices.resize(vertexBufferCount);

			staticMesh->GetVertices(vertices.data(), sizeof(DefaultVertex), vertexBufferCount);

			R32V3 min = mTransform->GetWorldRotation() * (vertices[0].Position * mTransform->GetWorldScale());
			R32V3 max = mTransform->GetWorldRotation() * (vertices[0].Position * mTransform->GetWorldScale());

			for (auto const& vertex : vertices)
			{
				R32V3 position = mTransform->GetWorldRotation() * (vertex.Position * mTransform->GetWorldScale());

				if (position.x < min.x) min.x = position.x;
				if (position.y < min.y) min.y = position.y;
				if (position.z < min.z) min.z = position.z;

				if (position.x > max.x) max.x = position.x;
				if (position.y > max.y) max.y = position.y;
				if (position.z > max.z) max.z = position.z;
			}

			mAABB = { min, max };
		}

		for (auto const& child : mChildren)
		{
			child->ComputeAABB();
		}
	}
}