#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/ObjSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	std::vector<ObjEntry> ObjSerializer::FromFile(const fs::path& File)
	{
		std::vector<ObjEntry> entries = {};

		BinaryReader binaryReader = { FsUtils::ReadBinary(File) };

		U32 size = binaryReader.Read<U32>();
		binaryReader.Read<ObjEntry>(entries, size);

		return entries;
	}

	void ObjSerializer::ToFile(const fs::path& File, const std::vector<ObjEntry>& Entries)
	{
		BinaryWriter binaryWriter = {};

		binaryWriter.Write<U32>((U32)Entries.size());
		binaryWriter.Write<ObjEntry>(Entries);

		FsUtils::WriteBinary(File, binaryWriter.GetBytes());
	}
}