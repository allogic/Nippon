#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
	class Scene;

	class Viewport : public Interface
	{
	public:

		Viewport(Scene* Scene);

	public:

		inline const auto& IsFocused() const { return mIsFocused; }

	public:

		virtual void Reset() override;
		virtual void Render() override;

	public:

		void SetFocused();

	private:

		bool HasResized();
		bool HasFocus();

	private:

		void HandleActorSelection();

	private:

		Scene* mScene;

		U32 mWidth = 0;
		U32 mHeight = 0;

		bool mIsFocused = false;
		bool mIsOpen = true;
	};
}
