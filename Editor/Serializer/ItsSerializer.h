#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ItsStructs.h>

namespace ark
{
	class ItsSerializer
	{
	public:

		static std::vector<ItsEntry> FromBytes(const std::vector<U8>& Bytes);
	};
}
