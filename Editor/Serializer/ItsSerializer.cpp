#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ItsSerializer.h>

namespace ark
{
	std::vector<ItsEntry> ItsSerializer::FromBytes(const std::vector<U8>& Bytes)
	{
		std::vector<ItsEntry> entries = {};

		BinaryReader binaryReader = Bytes;

		U32 size = binaryReader.Read<U32>();
		binaryReader.Read<ItsEntry>(entries, size);

		return entries;
	}
}
