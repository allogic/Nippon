#pragma once

#include <vector>
#include <cmath>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class ByteUtils
	{
	public:

		static U64 Compare(const std::vector<U8>& Lhs, const std::vector<U8>& Rhs);
		static std::vector<U64> Search(const std::vector<U8>& Bytes, const std::vector<U8>& Pattern);
	};
}