#include <string>
#include <map>
#include <cassert>

#include <Common/Utils/FileUtils.h>

#include <Editor/UI/AssetDatabase.h>

#include <Editor/Serializer/ObjectSerializer.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern std::map<std::string, ark::UI*> gUserInterface;

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
      object.SetPosition(objEntry.Position);
      object.SetRotation(objEntry.Rotation);
      object.SetScale(objEntry.Scale);

      ((AssetDatabase*)gUserInterface["assetDatabase"])->AddObject(object);
    }
  }
}
