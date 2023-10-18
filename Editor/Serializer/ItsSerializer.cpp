#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ItsSerializer.h>

namespace ark
{
	std::vector<ItsEntry> ItsSerializer::FromBytes(const std::vector<U8>& Bytes)
	{
		std::vector<ItsEntry> entries = {};

		BinaryReader reader = { Bytes.data(), Bytes.data() + Bytes.size() };

		U32 entryCount = reader.Read<U32>();

		reader.Read<ItsEntry>(entries, entryCount);

		return entries;
	}
}
