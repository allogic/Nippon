#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
	class Scene;
	class Actor;

	class Outline : public Interface
	{
	public:

		virtual void Reset() override;
		virtual void Render() override;

	public:

		const auto GetSelectedActor() const { return mSelectedActor; }

	public:

		void SetSelectedActor(Actor* Actor);

	private:

		void DrawActorRecursive(Scene* Scene, Actor* Actor);

	private:

		Actor* mSelectedActor = nullptr;
	};
}
