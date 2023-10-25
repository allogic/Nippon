#include <Common/BinaryMediator.h>

#include <Editor/Serializer/ItsSerializer.h>

namespace ark
{
	std::vector<ItsEntry> ItsSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		std::vector<ItsEntry> entries = {};

		BinaryMediator mediator = { Bytes, Size };

		U32 entryCount = mediator.Read<U32>();

		mediator.Read<ItsEntry>(entries, entryCount);

		return entries;
	}
}
