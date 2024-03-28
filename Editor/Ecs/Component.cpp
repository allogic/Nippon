#include <Ecs/Component.h>

namespace Nippon
{
	Component::Component(Scene* Scene, Entity* Entity)
	{
		mScene = Scene;
		mEntity = Entity;
	}

	Component::~Component()
	{

	}
}