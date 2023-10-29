#include <Common/BinaryMediator.h>

#include <Editor/Serializer/ObjectSerializer.h>

namespace ark
{
	std::vector<Object> ObjectSerializer::DeSerialize(U8* Bytes, U64 Size)
	{
		std::vector<Object> objects = {};

		BinaryMediator mediator = { Bytes, Size };

		U32 entryCount = mediator.Read<U32>();

		mediator.Read<Object>(objects, entryCount);

		return objects;
	}
}
