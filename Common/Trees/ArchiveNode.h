#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include <Common/Types.h>
#include <Common/BinaryReader.h>

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

  class ArchiveNode
  {
  public:

    ArchiveNode(const std::vector<U8>& Bytes);
    virtual ~ArchiveNode();

  public:

    inline auto IsArchive() const { return mIsArchive; }
    inline auto IsFile() const { return !mIsArchive; }

    inline auto GetCrc32() const { return mCrc32; }
    inline auto GetOffset() const { return mOffset; }
    inline auto GetSize() const { return mSize; }
    inline auto GetType() const { return mType; }
    inline auto GetName() const { return mName; }
    inline const auto& GetBytes() const { return mBinaryReader.GetBytes(); }

  public:

    inline void SetOffset(U32 Offset) { mOffset = Offset; };
    inline void SetSize(U32 Size) { mSize = Size;  };
    inline void SetType(const std::string& Type) { mType = Type; };
    inline void SetName(const std::string& Name) { mName = Name; };

  public:

    inline auto begin() { return mNodes.begin(); }
    inline const auto begin() const { return mNodes.cbegin(); }

    inline auto end() { return mNodes.end(); }
    inline const auto end() const { return mNodes.cend(); }

  private:

    void FetchHeader();
    bool ContainsArchive();

  private:

    BinaryReader mBinaryReader;
    const U32 mCrc32;
    const U32 mIsArchive;
    U32 mOffset;
    U32 mSize;
    std::string mType;
    std::string mName;
    std::vector<ArchiveEntry> mToc;
    std::multimap<std::string, ArchiveNode*> mNodes;
  };
}