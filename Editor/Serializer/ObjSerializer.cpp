#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ObjSerializer.h>

namespace ark
{
	std::vector<ObjEntry> ObjSerializer::FromBytes(const std::vector<U8>& Bytes)
	{
		std::vector<ObjEntry> entries = {};

		BinaryReader binaryReader = Bytes;

		U32 size = binaryReader.Read<U32>();
		binaryReader.Read<ObjEntry>(entries, size);

		return entries;
	}
}
