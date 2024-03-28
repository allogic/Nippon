#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class Viewport
	{
	public:

		Viewport(Scene* Scene);
		virtual ~Viewport();

	public:

		inline auto const& IsFocused() const { return mIsFocused; }

	public:

		void Reset();
		void Render();

	public:

		void SetFocused();

	private:

		bool HasResized();
		bool HasGainedFocus();
		bool HasFocus();
		bool HasLostFocus();

	private:

		void HandleEntitySelection();

	private:

		Scene* mScene = nullptr;

		bool mIsOpen = true;
		bool mIsFocused = false;

		U32 mWidth = 0;
		U32 mHeight = 0;
	};
}