#include <Editor/Assets/Model.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ModelEntry::ModelEntry(U32 Id, U32 Type)
    : mId{ Id }
    , mType{ Type }
  {

  }

  ModelGroup::ModelGroup(const std::string& Name)
    : mName{ Name }
  {

  }
}