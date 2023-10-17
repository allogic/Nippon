#include <Editor/Component.h>
#include <Editor/Actor.h>
#include <Editor/Scene.h>

namespace ark
{
	Component::Component(Scene* Scene, Actor* Actor)
		: mScene{ Scene }
		, mActor{ Actor }
	{

	}

	Component::~Component()
	{

	}
}
