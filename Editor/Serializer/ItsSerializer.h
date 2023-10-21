#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ItsStructs.h>

namespace ark
{
	class ItsSerializer
	{
	public:

		static std::vector<ItsEntry> FromBytes(U8* Bytes, U64 Size);
	};
}
