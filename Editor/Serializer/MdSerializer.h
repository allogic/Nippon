#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/MdStructs.h>

namespace ark
{
	class MdSerializer
	{
	public:

		static MdGroup FromBytes(const std::vector<U8>& Bytes);
	};
}
