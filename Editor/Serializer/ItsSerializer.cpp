#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ItsSerializer.h>

namespace ark
{
	std::vector<ItsEntry> ItsSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		std::vector<ItsEntry> entries = {};

		BinaryReader reader = { Bytes, Size };

		U32 entryCount = reader.Read<U32>();

		reader.Read<ItsEntry>(entries, entryCount);

		return entries;
	}
}
