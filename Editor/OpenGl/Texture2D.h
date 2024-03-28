#pragma once

#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class Texture2D
	{
	public:

		static U32 Create(U32 Width, U32 Height, U32 Channels, U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type, void const* Data, bool Compressed = false);

		static void Destroy(U32 Texture);
		static void Destroy(std::vector<U32> const& Textures);

	public:

		static U32 GetWidth(U32 Texture);
		static U32 GetHeight(U32 Texture);
		static U32 GetFormatInternal(U32 Texture);

	public:

		static void Bind(U32 Texture);
		static void Mount(U32 Index, U32 Texture);
		static void UnMount(U32 Index);
		static void UnBind();

	public:

		static std::vector<U8> CopyRGB(U32 Texture);
		static std::vector<U8> CopyRGBA(U32 Texture);
	};
}