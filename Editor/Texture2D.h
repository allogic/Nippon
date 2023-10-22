#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

namespace ark
{
	class Texture2D
	{
	public:

		static U32 Create(U32 Width, U32 Height, U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type, void* Data, bool Compressed = false);

		static void Destroy(U32 Id);
		static void Destroy(const std::vector<U32>& Ids);

	public:

		static U32 GetWidth(U32 Id);
		static U32 GetHeight(U32 Id);
		static U32 GetFormatInternal(U32 Id);

	public:

		static void Bind(U32 Id);
		static void Mount(U32 Index, U32 Id);
		static void UnMount(U32 Index);
		static void UnBind();

	public:

		static std::vector<U8> CopyRGB(U32 Id);
		static std::vector<U8> CopyRGBA(U32 Id);
	};
}
