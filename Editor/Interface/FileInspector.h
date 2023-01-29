#pragma once

#include <filesystem>

#include <Common/Trees/FileNode.h>

#include <Editor/Interface.h>

#include <Vendor/rapidjson/rapidjson.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class FileInspector : public Interface
  {
  public:

    virtual void Draw() override;

  private:

    void DrawFileNodeRecursive(FileNode* Node);

  private:

    FileNode* mFileNode = nullptr;
    FileNode* mSelectedNode = nullptr;
  };
}