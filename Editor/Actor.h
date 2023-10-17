#pragma once

#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <algorithm>

#include <Common/Types.h>

namespace ark
{
	class Scene;
	class Component;
	class Transform;

	class Actor
	{
	public:

		Actor(Scene* Scene, U32 Id, const std::string& Name);
		virtual ~Actor();

	public:

		inline const auto& IsActive() const { return mActive; }
		inline const auto& IsOpened() const { return mOpened; }
		inline const auto IsChild() const { return mChildren.size() == 0; }

		inline const auto& GetId() const { return mId; }
		inline const auto& GetName() const { return mName; }
		inline const auto& GetComponents() const { return mComponents; }
		inline auto& GetChildren() { return mChildren; }
		inline const auto& GetAABB() const { return mAABB; }
		inline const auto& GetParent() const { return mParent; }
		inline const auto& GetTransform() const { return mTransform; }
		inline const auto& GetShouldBeDestroyed() const { return mShouldBeDestroyed; }

	public:

		inline void SetParent(Actor* Parent) { mParent = Parent; }
		inline void SetAABB(const AABB& AABB) { mAABB = AABB; }

	public:

		virtual void Update() {}

	public:

		inline void AddChild(Actor* Child) { mChildren.emplace_back(Child); }
		inline void RemoveChild(Actor* Child) { mChildren.erase(std::find(mChildren.begin(), mChildren.end(), Child)); }

	public:

		void MakeActiveRecursiveUp(bool Value);
		void MakeActiveRecursiveDown(bool Value);

		void MakeOpenedRecursiveUp(bool Value);
		void MakeOpenedRecursiveDown(bool Value);

		void MakeShouldBeDestroyedRecursiveUp(bool Value);
		void MakeShouldBeDestroyedRecursiveDown(bool Value);

	public:

		void ComputeAABB();

	public:

		template<typename C, typename ... Args>
		C* AttachComponent(Args&& ... Arguments);

		template<typename C>
		C* GetComponent();

	protected:
		
		Scene* mScene;
		U32 mId;
		std::string mName;

		std::vector<Actor*> mChildren = {};
		std::map<U64, Component*> mComponents = {};

		Actor* mParent = nullptr;
		Transform* mTransform = nullptr;

		bool mActive = true;
		bool mOpened = false;
		bool mShouldBeDestroyed = false;

		AABB mAABB = {};
	};
}

namespace ark
{
	template<typename C, typename ... Args>
	C* Actor::AttachComponent(Args&& ... Arguments)
	{
		U64 hash = typeid(C).hash_code();

		const auto findIt = mComponents.find(hash);

		if (findIt == mComponents.end())
		{
			const auto [emplaceIt, inserted] = mComponents.emplace(hash, new C{ mScene, this, std::forward<Args>(Arguments) ... });

			return (C*)emplaceIt->second;
		}

		return (C*)findIt->second;
	}

	template<typename C>
	C* Actor::GetComponent()
	{
		return (C*)mComponents[typeid(C).hash_code()];
	}
}
