#pragma once

#include <Standard.h>
#include <Types.h>

#include <Archive/Structs/PlacementStructs.h>

namespace Nippon
{
	class PlacementSerializer
	{
	public:

		static std::vector<Object> ObjectsFromBytes(U8 const* Bytes, U64 Size);
	};
}