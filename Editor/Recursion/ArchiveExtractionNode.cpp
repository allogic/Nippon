#include <Common/Debug.h>
#include <Common/Crc32.h>

#include <Common/Utils/DirUtils.h>
#include <Common/Utils/FileUtils.h>
#include <Common/Utils/StringUtils.h>

#include <Editor/Recursion/ArchiveExtractionNode.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  static const std::set<std::string> sKnownArchiveTypes
  {
    "A00", "A01", "ACT", "AK", "AKT", "ANS",
    "B00", "B01", "BIN", "BMH",
    "C00", "CAM", "CCH", "CMP",
    "D00", "DAT", "DDP", "DDS",
    "EAR", "ECT", "EFF", "EFP", "EMD", "EST",
    "FI2", "FIS",
    "ICO", "IDD", "IDP", "ISL", "ITS",
    "JMP",
    "LI3",
    "MD", "MEH", "MOT", "MRT", "MSA", "MSD", "MSS",
    "PSN",
    "RHT", "RNI", "ROF",
    "S00", "S01", "S02", "S03", "S04", "S05", "SCA", "SCI", "SCL", "SCM", "SCP", "SCR", "SEH", "SEQ", "SES", "SSD", "SSL",
    "TAT", "TBL", "TRE", "TS", "TSC",
    "V00", "V01", "V02", "V03",
  };

  ArchiveExtractionNode::ArchiveExtractionNode(
    const std::vector<U8>& Bytes,
    ArchiveExtractionNode* Parent,
    U16 Index,
    U32 Offset,
    U32 Size,
    const std::string& Type,
    const std::string& Name)
    : mBinaryReader{ Bytes }
    , mParent{ Parent }
    , mIndex{ Index }
    , mOffset{ Offset }
    , mSize{ Size }
    , mType{ Type }
    , mName{ Name }
  {
    mIsDirectory = IsDirectory();

    if (mIsDirectory)
    {
      ReadHeader();

      for (U32 i = 0; i < mEntries.size(); i++)
      {
        mBinaryReader.SeekAbsolute(mEntries[i].Offset);

        ArchiveExtractionNode* node = mNodes.emplace_back(new ArchiveExtractionNode
        {
          mBinaryReader.Bytes(mEntries[i].Size),
          this,
          (U16)i,
          mEntries[i].Offset,
          mEntries[i].Size,
          mEntries[i].Type,
          mEntries[i].Name,
        });
      }
    }
  }

  ArchiveExtractionNode::~ArchiveExtractionNode()
  {
    for (auto& node : mNodes)
    {
      delete node;
      node = nullptr;
    }
  }

  void ArchiveExtractionNode::ExtractRecursive(U32 Count, const fs::path& File, ArchiveExtractionNode* Node)
  {
    if (!Node)
    {
      Node = this;
    }

    for (U32 i = 0; i < Count; i++)
    {
      if ((i % 2 == 0) && (i >= 2)) LOG("|");
      else LOG(" ");
    }

    LOG("%05u @ %20s @ %4s\n", Node->mIndex, Node->mName.c_str(), Node->mType.c_str());

    std::string fileName = GetFileName(Node);

    if (Node->mIsDirectory)
    {
      if (Node->mType[0] == 0)
      {
        for (const auto& node : Node->mNodes)
        {
          ExtractRecursive(Count + 2, File, node);
        }
      }
      else
      {
        DirUtils::CreateIfNotExists((File / fileName).string());

        for (const auto& node : Node->mNodes)
        {
          ExtractRecursive(Count + 2, (File / fileName).string(), node);
        }
      }
    }
    else
    {
      FileUtils::WriteBinary((File / fileName).string(), Node->GetBytes());
    }
  }

  void ArchiveExtractionNode::ReadHeader()
  {
    mBinaryReader.SeekAbsolute(0);

    mEntries.resize(mBinaryReader.Read<U32>());

    for (U32 i = 0; i < mEntries.size(); i++)
    {
      mEntries[i].Offset = mBinaryReader.Read<U32>();
    }

    for (U32 i = 0; i < mEntries.size(); i++)
    {
      mEntries[i].Type = StringUtils::RemoveNulls(mBinaryReader.String(4));
    }

    for (U32 i = 0; i < mEntries.size(); i++)
    {
      mBinaryReader.SeekAbsolute(mEntries[i].Offset);
      mBinaryReader.SeekRelative(-20);

      mEntries[i].Name = StringUtils::RemoveNulls(mBinaryReader.String(20));
    }

    for (U32 i = 1; i < mEntries.size(); i++)
    {
      mEntries[i - 1].Size = mEntries[i].Offset - mEntries[i - 1].Offset;

      if (mEntries[i - 1].Type != "ROF")
      {
        mEntries[i - 1].Size -= 24;
      }
    }

    mEntries[mEntries.size() - 1].Size = (U32)mBinaryReader.GetSize() - mEntries[mEntries.size() - 1].Offset;
  }

  bool ArchiveExtractionNode::IsDirectory()
  {
    if (mBinaryReader.GetSize() > 0)
    {
      mBinaryReader.SeekAbsolute(0);

      U32 size = mBinaryReader.Read<U32>();

      if (size > 0 && size < 65535)
      {
        for (U32 i = 0; i < size; i++)
        {
          U32 offset = mBinaryReader.Read<U32>();

          if (offset >= mBinaryReader.GetSize())
          {
            return false;
          }
        }

        for (U32 i = 0; i < size; i++)
        {
          std::string type = StringUtils::RemoveNulls(mBinaryReader.String(4));

          if (!sKnownArchiveTypes.contains(type))
          {
            return false;
          }
        }

        return true;
      }
    }

    return false;
  }

  std::string ArchiveExtractionNode::GetFileName(ArchiveExtractionNode* Node) const
  {
    char fileName[32] = {};

    std::snprintf(fileName, 6, "%05u", Node->mIndex);
    fileName[5] = '@';
    std::memcpy(&fileName[6], &Node->mName[0], Node->mName.size());
    fileName[6 + Node->mName.size()] = '@';
    std::memcpy(&fileName[7 + Node->mName.size()], &Node->mType[0], Node->mType.size());

    return fileName;
  }
}