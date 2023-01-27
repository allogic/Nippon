#include <Common/ExtensionIterator.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ExtensionIterator::ExtensionIterator(const fs::path& File, const std::string& Ext)
  {
    if (fs::is_directory(File))
    {
      for (const auto& file : fs::directory_iterator{ File })
      {
        if (file.path().extension().string() == Ext)
        {
          mFiles.emplace_back(file);
        }
      }
    }
  }
}