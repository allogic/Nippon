#pragma once

#include <string>

#include <Common/Types.h>

#include <Editor/Actor.h>

namespace ark
{
	class Test : public Actor
	{
	public:

		Test(Scene* Scene, U32 Id, const std::string& Name);

	public:

		void Update() override;
	};
}
