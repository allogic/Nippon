#include <Ecs/Registry.h>
#include <Ecs/Entity.h>

#include <Ecs/Components/Camera.h>
#include <Ecs/Components/Transform.h>
#include <Ecs/Components/Renderable.h>

#include <Ecs/Entities/Player.h>

#include <Interface/Outline.h>

#include <Renderer/Renderer.h>

namespace Nippon
{
	Registry::Registry(Scene* Scene)
	{
		mScene = Scene;

		mRootEntity = CreateEntity("Root", nullptr);
		mPlayerEntity = CreateEntity<Player>("Player", nullptr);

		mMainCamera = mPlayerEntity->GetComponent<Camera>();
	}

	Registry::~Registry()
	{
		DestroySimple();
	}

	void Registry::UpdateEntity(Entity* Entity, bool Recursive)
	{
		if (!Entity)
		{
			Entity = mRootEntity;
		}

		if (Entity)
		{
			if (Recursive)
			{
				for (auto const& child : Entity->GetChildren())
				{
					UpdateEntity(child, Recursive);
				}
			}

			Entity->Update();
		}
	}

	std::vector<Entity*> Registry::DestroyEntity(Entity* Entity, bool Recursive)
	{
		if (!Entity)
		{
			Entity = mRootEntity;
		}

		if (Entity)
		{
			if (Recursive)
			{
				auto& children = Entity->GetChildren();

				for (auto it = children.begin(); it != children.end();)
				{
					bool removeChild = (*it)->GetShouldBeDestroyed() && (*it)->GetParent();

					auto const childChildren = DestroyEntity(*it, Recursive);

					if (removeChild)
					{
						it = children.insert(it, childChildren.begin(), childChildren.end());
						it += childChildren.size();
						it = children.erase(it);
					}
					else
					{
						it++;
					}
				}
			}

			if (Entity->GetShouldBeDestroyed())
			{
				auto const children = Entity->GetChildren();

				for (auto const& child : children)
				{
					child->SetParent(Entity->GetParent());
				}

				if (Entity == mRootEntity)
				{
					mRootEntity = nullptr;
				}

				if (Entity == mPlayerEntity)
				{
					mMainCamera = nullptr;
				}

				if (Entity == Outline::GetSelectedEntity())
				{
					Outline::SetSelectedEntity(nullptr);
				}

				Entity->~Entity();

				Memory::Free(Entity);

				return children;
			}
		}

		return {};
	}

	Entity* Registry::FindEntityByUniqueId(U32 UniqueId, Entity* Entity, bool Recursive)
	{
		if (!Entity)
		{
			Entity = mRootEntity;
		}

		if (Entity)
		{
			if (Recursive)
			{
				for (auto const& child : Entity->GetChildren())
				{
					if (auto const& entity = FindEntityByUniqueId(UniqueId, child, Recursive))
					{
						return entity;
					}
				}
			}

			if (UniqueId == Entity->GetUniqueId())
			{
				return Entity;
			}
		}

		return nullptr;
	}

	void Registry::SubmitEntityToRenderer(Renderer* Renderer, Entity* Entity, bool Recursive)
	{
		if (!Entity)
		{
			Entity = mRootEntity;
		}

		if (Entity && Entity->IsActive())
		{
			if (Recursive)
			{
				for (auto const& child : Entity->GetChildren())
				{
					SubmitEntityToRenderer(Renderer, child, Recursive);
				}
			}

			Transform* transform = Entity->GetTransform();
			Renderable* renderable = Entity->GetComponent<Renderable>();
			
			if (transform && renderable)
			{
				Renderer->SubmitToDefaultQueue(transform, renderable);
			}
		}
	}

	void Registry::HandleEntitySelection(Renderer* Renderer, Entity* Entity, bool Recursive)
	{
		if (!Entity)
		{
			Entity = mRootEntity;
		}

		if (Entity && Entity->IsActive())
		{
			if (Recursive)
			{
				for (const auto& child : Entity->GetChildren())
				{
					HandleEntitySelection(Renderer, child, Recursive);
				}
			}

			Transform* transform = Entity->GetComponent<Transform>();

			if (transform)
			{
				Renderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
				Renderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
				Renderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

				Renderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
				Renderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
				Renderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

				Renderer->DebugAABB(transform->GetWorldPosition(), Entity->GetAABB(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
			}
		}
	}

	void Registry::DestroySimple(Entity* Entity)
	{
		if (!Entity)
		{
			Entity = mRootEntity;
		}

		if (Entity)
		{
			for (const auto& child : Entity->GetChildren())
			{
				DestroySimple(child);
			}

			if (Entity == mRootEntity)
			{
				mRootEntity = nullptr;
			}

			if (Entity == mPlayerEntity)
			{
				mMainCamera = nullptr;
			}

			if (Entity == Outline::GetSelectedEntity())
			{
				Outline::SetSelectedEntity(nullptr);
			}

			Entity->~Entity();

			Memory::Free(Entity);
		}
	}
}