#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class Component
	{
	public:

		Component(Scene* Scene, Entity* Entity);
		virtual ~Component();

	public:

		inline auto const& GetScene() const { return mScene; }
		inline auto const& GetEntity() const { return mEntity; }

	private:

		Scene* mScene = nullptr;
		Entity* mEntity = nullptr;
	};
}