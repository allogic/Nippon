#include <Archive/Serializer/ItsSerializer.h>

#include <Common/BinaryMediator.h>

namespace Nippon
{
	std::vector<ItsEntry> ItsSerializer::FromBytes(U8 const* Bytes, U64 Size)
	{
		std::vector<ItsEntry> entries = {};

		BinaryMediator mediator = { Bytes, Size };

		U32 entryCount = mediator.Read<U32>();

		mediator.Read<ItsEntry>(entries, entryCount);

		return entries;
	}
}