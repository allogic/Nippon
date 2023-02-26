#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <filesystem>

#include <Common/Types.h>
#include <Common/BinaryReader.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  struct ArchiveEntry
  {
    U32 Offset;
    U32 Size;
    std::string Type;
    std::string Name;
  };

  class ArchiveExtractionNode
  {
  public:

    ArchiveExtractionNode(
      const std::vector<U8>& Bytes,
      ArchiveExtractionNode* Parent,
      U16 Index,
      U32 Offset,
      U32 Size,
      const std::string& Type,
      const std::string& Name,
      bool IsDirectory);
    virtual ~ArchiveExtractionNode();

  public:

    void ExtractRecursive(U32 Count, fs::path File, ArchiveExtractionNode* Node = nullptr, bool Verbose = false);

  private:

    void ReadHeader();
    bool ValidateDirectory(const ArchiveEntry& Entry);

  private:

    BinaryReader mBinaryReader;
    ArchiveExtractionNode* mParent;
    U16 mIndex;
    U32 mOffset;
    U32 mSize;
    std::string mType;
    std::string mName;
    bool mIsDirectory;

    std::vector<ArchiveEntry> mEntries = {};
    std::vector<ArchiveExtractionNode*> mNodes = {};
  };
}