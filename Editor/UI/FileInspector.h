#pragma once

#include <filesystem>

#include <Common/Trees/FileNode.h>

#include <Editor/UI.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class FileInspector : public UI
  {
  public:

    virtual void Update() override;
    virtual void Draw() override;

  private:

    void DrawFileNodeRecursive(FileNode* Node);

  private:

    FileNode* mFileNode = nullptr;
    FileNode* mSelectedNode = nullptr;
  };
}