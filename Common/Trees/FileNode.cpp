#include <Common/Trees/FileNode.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  FileNode::FileNode(const fs::path& File)
    : mFile{ File }
  {
    if (fs::is_directory(mFile))
    {
      for (const auto& file : fs::directory_iterator{ mFile })
      {
        mNodes[file] = new FileNode{ file };
      }
    }
  }

  FileNode::~FileNode()
  {
    for (auto& [file, node] : mNodes)
    {
      delete node;
      node = nullptr;
    }
  }
}