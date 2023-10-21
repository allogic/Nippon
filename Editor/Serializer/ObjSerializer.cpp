#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ObjSerializer.h>

namespace ark
{
	std::vector<ObjEntry> ObjSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		std::vector<ObjEntry> entries = {};

		BinaryReader reader = { Bytes, Size };

		U32 entryCount = reader.Read<U32>();

		reader.Read<ObjEntry>(entries, entryCount);

		return entries;
	}
}
