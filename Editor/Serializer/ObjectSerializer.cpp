#include <cassert>

#include <Common/Utils/FileUtils.h>

#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>

#include <Editor/Serializer/ObjectSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ObjectSerializer::ObjectSerializer(Scene* Scene, const fs::path& File)
    : mBinaryReader{ FileUtils::ReadBinary(File.string()) }
  {
    U32 size = mBinaryReader.Read<U32>();

    for (U32 i = 0; i < size; i++)
    {
      ObjEntry object = mBinaryReader.Read<ObjEntry>();

      Actor* actor = Scene->CreateActor<Actor>(std::to_string(object.Id), nullptr);
      
      actor->GetTransform()->SetWorldPosition(R32V3{ object.Position.x, object.Position.y, object.Position.z });
      actor->GetTransform()->SetWorldRotation(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z } / 255.0F * 360.0F);
      actor->GetTransform()->SetWorldScale(R32V3{ object.Scale.x, object.Scale.y, object.Scale.z });
    }
  }
}
