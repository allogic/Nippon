#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/CameraController.h>

namespace ark
{
	CameraController::CameraController(Scene* Scene, Actor* Actor)
		: Component{ Scene, Actor }
	{

	}
}
