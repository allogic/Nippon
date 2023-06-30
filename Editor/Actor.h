#pragma once

#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <algorithm>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Math.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class Actor
	{
	public:

		Actor(Scene* Scene, const std::string& Name);
		virtual ~Actor();

	public:

		inline auto IsActive() const { return mActive; }
		inline auto IsChild() const { return mChildren.size() == 0; }
		inline auto HasParent() const { return mParent != nullptr; }

		inline const auto& GetName() const { return mName; }
		inline const auto& GetComponents() const { return mComponents; }
		inline const auto& GetChildren() const { return mChildren; }
		inline const auto& GetAABB() const { return mAABB; }

		inline auto GetParent() const { return mParent; }
		inline auto GetTransform() const { return mTransform; }

	public:

		void SetActive(bool Active);

		inline void SetParent(Actor* Parent) { mParent = Parent; }
		inline void SetAABB(const AABB& AABB) { mAABB = AABB; }

	public:

		virtual void Update(R32 TimeDelta) {}

	public:

		inline void AddChild(Actor* Child) { mChildren.emplace_back(Child); }
		inline void RemoveChild(Actor* Child) { mChildren.erase(std::find(mChildren.begin(), mChildren.end(), Child)); }

	public:

		template<typename C, typename ... Args>
		C* AttachComponent(Args&& ... Arguments);

		template<typename C>
		C* GetComponent();

	protected:
		
		Scene* mScene;
		std::string mName;
		std::vector<Actor*> mChildren;
		std::map<U64, Component*> mComponents;

		Actor* mParent = nullptr;
		Transform* mTransform = nullptr;

		bool mActive = true;

		AABB mAABB = {};
	};
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	template<typename C, typename ... Args>
	C* Actor::AttachComponent(Args&& ... Arguments)
	{
		U64 hash = typeid(C).hash_code();
		auto const findIt = mComponents.find(hash);
		if (findIt == mComponents.end())
		{
			auto const [emplaceIt, inserted] = mComponents.emplace(hash, new C{ mScene, this, std::forward<Args>(Arguments) ... });
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