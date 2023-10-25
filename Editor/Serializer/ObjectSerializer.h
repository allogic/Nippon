#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ObjectStructs.h>

namespace ark
{
	class ObjectSerializer
	{
	public:

		static std::vector<ObjEntry> DeSerialize(U8* Bytes, U64 Size);
	};
}
