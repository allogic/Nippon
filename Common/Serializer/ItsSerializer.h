#pragma once

#include <Common/Standard.h>
#include <Common/Forward.h>
#include <Common/Types.h>

#include <Common/Structs/ItsStructs.h>

namespace Nippon
{
	class ItsSerializer
	{
	public:

		static std::vector<ItsEntry> FromBytes(U8 const* Bytes, U64 Size);
	};
}