#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/MdStructs.h>

namespace ark
{
	class MdSerializer
	{
	public:

		static MdGroup FromBytes(U8* Bytes, U64 Size);
	};
}
