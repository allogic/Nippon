#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class Outline
	{
	public:

		static void Reset();
		static void Render();

	public:

		static Entity* GetSelectedEntity();

	public:

		static void SetSelectedEntity(Entity* Entity);

	private:

		static bool DrawEntityTreeRecursive(Scene* Scene, Entity* Entity);
	};
}