#pragma once

#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

namespace Nippon
{
	class TextureUtility
	{
	public:

		static U32 ReadDDS(fs::path const& File);
		static U32 ReadDDS(U8 const* Bytes, U64 Size);

		static U32 ReadPNG(fs::path const& File);
		static U32 ReadPNG(U8 const* Bytes, U64 Size);

		static void WritePNG(U32 TextureId, fs::path const& File);
		static std::vector<U8> WritePNG(U32 TextureId);
	};
}