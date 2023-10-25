#include <Common/BinaryMediator.h>

#include <Editor/Serializer/ObjSerializer.h>

namespace ark
{
	std::vector<ObjEntry> ObjSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		std::vector<ObjEntry> entries = {};

		BinaryMediator mediator = { Bytes, Size };

		U32 entryCount = mediator.Read<U32>();

		mediator.Read<ObjEntry>(entries, entryCount);

		return entries;
	}
}
