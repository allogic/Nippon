#pragma once

#include <utility>
#include <vector>
#include <cmath>

#include <Common/Types.h>

namespace ark
{
	class DiffUtils
	{
	public:

		static bool Compare(const std::vector<U8>& Left, const std::vector<U8>& Right, std::vector<std::pair<U64, U64>>& Indices);
		static void HexDump(const std::vector<U8>& Left, const std::vector<U8>& Right, const std::vector<std::pair<U64, U64>>& Indices, U32 ResultCount = 5, U64 Size = 64, U32 Stride = 16, U32 LeadStrideMultiplier = 3);
	};
}
