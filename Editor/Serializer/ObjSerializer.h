#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ObjStructs.h>

namespace ark
{
	class ObjSerializer
	{
	public:

		static std::vector<ObjEntry> FromBytes(const std::vector<U8>& Bytes);
	};
}
