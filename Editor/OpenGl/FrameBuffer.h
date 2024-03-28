#pragma once

#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class FrameBuffer
	{
	public:

		static U32 Create(U32 Width, U32 Height);
		static void Destroy(U32 FrameBuffer);

	public:

		static U32 GetWidth(U32 FrameBuffer);
		static U32 GetHeight(U32 FrameBuffer);
		static U32 GetColorTexture(U32 FrameBuffer, U32 Index);
		static U32 GetDepthStencilTexture(U32 FrameBuffer);

	public:

		static U32 ReadInteger(U32 FrameBuffer, U32 X, U32 Y);

	public:

		static std::vector<U8> CopyRGB(U32 FrameBuffer);
		static std::vector<U8> CopyRGBA(U32 FrameBuffer);
	};
}