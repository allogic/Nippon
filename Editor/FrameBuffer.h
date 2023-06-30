#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class FrameBuffer
	{
	public:

		FrameBuffer();
		virtual ~FrameBuffer();

	public:

		inline auto GetId() const { return mFbo; }
		inline const auto GetColorTexture() const { return mColorTexture; }
		inline const auto GetDepthStencilTexture() const { return mDepthStencilTexture; }

	public:

		void Resize(U32 Width, U32 Height);

	private:

		U32 mFbo = 0;

		RenderTexture* mColorTexture = nullptr;
		DepthStencilTexture* mDepthStencilTexture = nullptr;
	};
}