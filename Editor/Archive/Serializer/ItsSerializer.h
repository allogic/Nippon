#pragma once

#include <Standard.h>
#include <Types.h>

#include <Archive/Structs/ItsStructs.h>

namespace Nippon
{
	class ItsSerializer
	{
	public:

		static std::vector<ItsEntry> FromBytes(U8 const* Bytes, U64 Size);
	};
}