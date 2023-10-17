#pragma once

namespace ark
{
	class Scene;
	class Actor;

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
