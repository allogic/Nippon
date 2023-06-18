#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/ObjectSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<Object> ObjectSerializer::FromFile(const fs::path& File)
  {
    std::vector<Object> objects = {};

    BinaryReader binaryReader{ FsUtils::ReadBinary(File) };

    U32 size = binaryReader.Read<U32>();
    binaryReader.Read<Object>(objects, size);

    return objects;
  }

  void ObjectSerializer::ToFile(const fs::path& File, const std::vector<Object>& Objects)
  {
    BinaryWriter binaryWriter = {};

    binaryWriter.Write<U32>((U32)Objects.size());
    binaryWriter.Write<Object>(Objects);

    FsUtils::WriteBinary(File, binaryWriter.GetBytes());
  }
}