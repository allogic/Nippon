#pragma once

#include <Forward.h>
#include <Memory.h>
#include <Standard.h>
#include <Types.h>

#include <Math/AABB.h>

namespace Nippon
{
	class Entity
	{
	public:

		Entity(Scene* Scene, U32 UniqueId, std::string const& Name);
		virtual ~Entity();

	public:

		inline const auto& IsActive() const { return mActive; }
		inline const auto& IsOpened() const { return mOpened; }

		inline const auto IsChild() const { return mChildren.size() == 0; }

		inline auto const& GetUniqueId() const { return mUniqueId; }
		inline auto const& GetParent() const { return mParent; }
		inline auto const& GetScene() const { return mScene; }
		inline auto const& GetName() const { return mName; }
		inline auto const& GetTransform() const { return mTransform; }
		inline auto const& GetAABB() const { return mAABB; }

		inline auto& GetChildren() { return mChildren; }

		inline auto const GetShouldBeDestroyed() const { return mShouldBeDestroyed; }

	public:

		inline void SetParent(Entity* Parent) { mParent = Parent; }

		void SetShouldBeDestroyed();
		void SetShouldBeDestroyedUp();
		void SetShouldBeDestroyedDown();

	public:

		void SetActiveUp(bool Value);
		void SetActiveDown(bool Value);

		void SetOpenedUp(bool Value);
		void SetOpenedDown(bool Value);

	public:

		virtual void Update();

	public:

		template<typename C, typename ... Args>
		C* AttachComponent(Args&& ... Arguments);

		template<typename C>
		C* GetComponent();

	public:

		void AddChild(Entity* Entity);
		void RemoveChild(Entity* Entity);

	public:

		void ComputeAABB();

	private:

		Scene* mScene = nullptr;

		U32 mUniqueId = 0;

		std::string mName = "";

		Entity* mParent = nullptr;

		std::vector<Entity*> mChildren = {};
		std::map<U64, Component*> mComponents = {};

		bool mActive = true;
		bool mOpened = false;
		bool mShouldBeDestroyed = false;

		AABB mAABB = {};

	private:

		Transform* mTransform = nullptr;
	};
}

namespace Nippon
{
	template<typename C, typename ... Args>
	C* Entity::AttachComponent(Args&& ... Arguments)
	{
		U64 hash = typeid(C).hash_code();

		auto const& findIt = mComponents.find(hash);

		if (findIt == mComponents.end())
		{
			Component* component = new (Memory::Alloc(sizeof(C))) C{ mScene, this, std::forward<Args>(Arguments) ... };

			mComponents.emplace(hash, component);

			return (C*)component;
		}

		return (C*)findIt->second;
	}

	template<typename C>
	C* Entity::GetComponent()
	{
		U64 hash = typeid(C).hash_code();

		return mComponents.contains(hash) ? (C*)mComponents[hash] : nullptr;
	}
}