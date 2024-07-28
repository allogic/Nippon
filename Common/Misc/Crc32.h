#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

namespace Nippon
{
	class Crc32
	{
	public:

		static void ComputeLuT(std::vector<U32>& Table, U32 Polynomial);

	public:

		static U32 FromRange(U8 const* Data, U64 Size);
	};
}