#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

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