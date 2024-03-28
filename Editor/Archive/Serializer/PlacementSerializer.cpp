#include <Archive/Serializer/PlacementSerializer.h>

#include <Common/BinaryMediator.h>

namespace Nippon
{
	std::vector<Object> PlacementSerializer::ObjectsFromBytes(U8 const* Bytes, U64 Size)
	{
		std::vector<Object> objects = {};

		BinaryMediator mediator = { Bytes, Size };

		U32 entryCount = mediator.Read<U32>();

		mediator.Read<Object>(objects, entryCount);

		return objects;
	}
}