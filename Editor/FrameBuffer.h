#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	class FrameBuffer
	{
	public:

		static U32 Create(U32 Width, U32 Height);
		static void Destroy(U32 Id);

	public:

		static U32 GetColorTexture(U32 Id, U32 Index);
		static U32 GetDepthStencilTexture(U32 Id);

	public:

		static U32 ReadIntegerAt(U32 Id, U32 X, U32 Y, U32 Index);

	public:

		static std::vector<U8> CopyRGB(U32 Id);
		static std::vector<U8> CopyRGBA(U32 Id);
	};
}
