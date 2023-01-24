#include <Common/Utils/DirUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void DirUtils::CreateIfNotExists(const fs::path& File)
  {
    if (!fs::exists(File))
    {
      fs::create_directory(File);
    }
  }
}