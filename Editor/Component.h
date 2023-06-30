#pragma once

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class Component
	{
	public:

		Component(Scene* Scene, Actor* Actor);
		virtual ~Component();

	protected:

		Scene* mScene;
		Actor* mActor;
	};
}