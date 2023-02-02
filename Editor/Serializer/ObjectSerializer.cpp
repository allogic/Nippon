#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Editor/Serializer/ObjectSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<Object> ObjectSerializer::ToObjects(const std::vector<U8>& Bytes)
  {
    std::vector<Object> objects = {};

    BinaryReader binaryReader{ Bytes };

    U32 size = binaryReader.Read<U32>();
    binaryReader.Read<Object>(objects, size);

    return objects;
  }

  std::vector<U8> ObjectSerializer::ToBytes(const std::vector<Object>& Objects)
  {
    BinaryWriter binaryWriter = {};

    binaryWriter.Write<U32>((U32)Objects.size());
    binaryWriter.Write<Object>(Objects);

    return binaryWriter.GetBytes();
  }
}