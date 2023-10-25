#include <Common/BinaryMediator.h>

#include <Editor/Serializer/ObjectSerializer.h>

namespace ark
{
	std::vector<ObjEntry> ObjectSerializer::DeSerialize(U8* Bytes, U64 Size)
	{
		std::vector<ObjEntry> objects = {};

		BinaryMediator mediator = { Bytes, Size };

		U32 entryCount = mediator.Read<U32>();

		mediator.Read<ObjEntry>(objects, entryCount);

		return objects;
	}
}
