#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ObjSerializer.h>

namespace ark
{
	std::vector<ObjEntry> ObjSerializer::FromBytes(const std::vector<U8>& Bytes)
	{
		std::vector<ObjEntry> entries = {};

		BinaryReader reader = { Bytes.data(), Bytes.data() + Bytes.size() };

		U32 entryCount = reader.Read<U32>();

		reader.Read<ObjEntry>(entries, entryCount);

		return entries;
	}
}
