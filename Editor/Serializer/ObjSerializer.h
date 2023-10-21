#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ObjStructs.h>

namespace ark
{
	class ObjSerializer
	{
	public:

		static std::vector<ObjEntry> FromBytes(U8* Bytes, U64 Size);
	};
}
