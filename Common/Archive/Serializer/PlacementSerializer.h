#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Archive/Structs/PlacementStructs.h>

namespace Nippon
{
	class PlacementSerializer
	{
	public:

		static std::vector<Object> ObjectsFromBytes(U8 const* Bytes, U64 Size);
	};
}