#pragma once

#include <string>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class Box : public Actor
	{
	public:

		Box(Scene* Scene, std::string const& Name);

	public:

		void Update(R32 TimeDelta) override;
	};
}