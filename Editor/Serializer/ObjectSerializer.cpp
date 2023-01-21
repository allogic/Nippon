#include <cassert>

#include <Common/Utils/FileUtils.h>

#include <Editor/AssetDatabase.h>

#include <Editor/Serializer/ObjectSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ObjectSerializer::ObjectSerializer(const fs::path& File)
    : mFile{ File }
    , mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    U32 size = mBinaryReader.Read<U32>();

    for (U32 i = 0; i < size; i++)
    {
      ObjEntry objEntry = mBinaryReader.Read<ObjEntry>();

      Object object;

      object.SetId(objEntry.Id);
      object.SetCategory(objEntry.Category);
      object.SetPosition(R32V3{ objEntry.Position.x, objEntry.Position.y, objEntry.Position.z });
      object.SetRotation(R32V3{ objEntry.Rotation.x, objEntry.Rotation.y, objEntry.Rotation.z });
      object.SetScale(R32V3{ objEntry.Scale.x, objEntry.Scale.y, objEntry.Scale.z });

      AssetDatabase::AddObject(object);
    }
  }
}
