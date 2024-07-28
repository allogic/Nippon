#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

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