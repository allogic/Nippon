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
      const std::string& Name);
    virtual ~ArchiveExtractionNode();

  public:

    inline const auto& GetBytes() const { return mBinaryReader.GetBytes(); }

  public:

    void ExtractRecursive(U32 Count, const fs::path& File, ArchiveExtractionNode* Node = nullptr, bool Verbose = false);

  private:

    void ReadHeader();
    bool IsDirectory();

    std::string GetFileName(ArchiveExtractionNode* Node) const;

  private:

    BinaryReader mBinaryReader;
    ArchiveExtractionNode* mParent;
    U16 mIndex;
    U32 mOffset;
    U32 mSize;
    std::string mType;
    std::string mName;

    bool mIsDirectory = false;

    struct ArchiveEntry
    {
      U32 Offset;
      U32 Size;
      std::string Type;
      std::string Name;
    };

    std::vector<ArchiveEntry> mEntries = {};
    std::vector<ArchiveExtractionNode*> mNodes = {};
  };
}