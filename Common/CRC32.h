#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

namespace ark
{
	class CRC32
	{
	public:

		static void ComputeLuT(std::vector<U32>& Table, U32 Polynomial);

	public:

		static U32 FromRange(const U8* Data, U64 Size);
	};
}