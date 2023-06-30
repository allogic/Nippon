#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/ItsSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<ItsEntry> ItsSerializer::FromFile(const fs::path& File)
  {
    std::vector<ItsEntry> entries = {};

    BinaryReader binaryReader = { FsUtils::ReadBinary(File) };

    U32 size = binaryReader.Read<U32>();
    binaryReader.Read<ItsEntry>(entries, size);

    return entries;
  }

  void ItsSerializer::ToFile(const fs::path& File, const std::vector<ItsEntry>& Entries)
  {
    BinaryWriter binaryWriter = {};

    binaryWriter.Write<U32>((U32)Entries.size());
    binaryWriter.Write<ItsEntry>(Entries);

    FsUtils::WriteBinary(File, binaryWriter.GetBytes());
  }
}