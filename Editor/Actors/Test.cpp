#include <Common/Macros.h>

#include <Editor/Actor.h>
#include <Editor/Editor.h>

#include <Editor/Actors/Test.h>

#include <Editor/Components/Transform.h>

#include <Editor/Renderer/DebugRenderer.h>

#include <Vendor/GLM/gtx/euler_angles.hpp>

namespace ark
{
	Test::Test(Scene* Scene, U32 Id, const std::string& Name)
		: Actor{ Scene, Id, Name }
	{

	}

	void Test::Update()
	{
		static R32 time = 0.0F;

		time += gTimeDelta * 0.1F;

		if (Transform* transform = GetTransform())
		{
			transform->SetLocalPosition(R32V3{ glm::sin(time) * 5.0F, 0.0F, glm::cos(time) * 5.0F });

			gDebugRenderer->DebugGrid(transform->GetWorldPosition(), 10, 3.0F, R32V4{ 1.0F, 1.0F, 0.0F, 1.0F }, transform->GetWorldRotation());
		}
	}
}
