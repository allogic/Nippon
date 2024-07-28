#pragma once

#include <Common/Forward.h>
#include <Common/Memory.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

#include <Editor/Ecs/Entity.h>

namespace Nippon
{
	class Registry
	{
	public:

		Registry(Scene* Scene);
		virtual ~Registry();

	public:

		inline auto const& GetRootEntity() const { return mRootEntity; }
		inline auto const& GetPlayerEntity() const { return mPlayerEntity; }
		inline auto const& GetMainCamera() const { return mMainCamera; }

	public:

		template<typename E = Entity, typename ... Args>
		E* CreateEntity(std::string const& Name, Entity* Parent, Args&& ... Arguments);

	public:

		void UpdateEntity(Entity* Entity = nullptr, bool Recursive = false);
		std::vector<Entity*> DestroyEntity(Entity* Entity, bool Recursive = false);
		Entity* FindEntityByUniqueId(U32 UniqueId, Entity* Entity = nullptr, bool Recursive = false);
		void SubmitEntityToRenderer(Renderer* Renderer, Entity* Entity = nullptr, bool Recursive = false);
		void HandleEntitySelection(Renderer* Renderer, Entity* Entity = nullptr, bool Recursive = false);

	private:

		void DestroySimple(Entity* Entity = nullptr);

	public:

		Scene* mScene = nullptr;

		U32 mUniqueEntityId = 0;

		Entity* mRootEntity = nullptr;
		Entity* mPlayerEntity = nullptr;

		Camera* mMainCamera = nullptr;
	};
}

namespace Nippon
{
	template<typename E, typename ... Args>
	E* Registry::CreateEntity(std::string const& Name, Entity* Parent, Args&& ... Arguments)
	{
		Entity* parent = Parent ? Parent : mRootEntity;
		Entity* entity = new (Memory::Alloc(sizeof(E))) E{ mScene, mUniqueEntityId++, Name, std::forward<Args>(Arguments) ... };

		entity->SetParent(parent);

		if (parent)
		{
			parent->AddChild(entity);
		}

		return (E*)entity;
	}
}