#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ScrStructs.h>

namespace ark
{
	class ScrSerializer
	{
	public:

		static ScrGroup FromBytes(U8* Bytes, U64 Size);
	};
}
