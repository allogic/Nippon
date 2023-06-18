#include <Common/Alignment.h>
#include <Common/Debug.h>

#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>

#include <Common/Recursion/ArchiveExtractionNode.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  static const std::set<std::string> sKnownDirectoryTypes
  {
    "AKT", "BIN", "CMP", "DAT", "DDP", "EFF", "IDD", "SCP", "TBL",
  };

  static const std::set<std::string> sKnownFileTypes
  {
    "A00", "A01", "ACT", "AK", "ANS",
    "B00", "B01", "BMH",
    "C00", "CAM", "CCH",
    "D00", "DDS",
    "EAR", "ECT", "EFP", "EMD", "EST",
    "FI2", "FIS",
    "ICO", "IDP", "ISL", "ITS",
    "JMP",
    "LI3",
    "MD", "MEH", "MOT", "MRT", "MSA", "MSD", "MSS",
    "PSN",
    "RHT", "RNI", "ROF",
    "S00", "S01", "S02", "S03", "S04", "S05", "SCA", "SCI", "SCL", "SCM", "SCR", "SEH", "SEQ", "SES", "SSD", "SSL",
    "TAT", "TRE", "TS", "TSC",
    "V00", "V01", "V02", "V03",
  };

  ArchiveExtractionNode::ArchiveExtractionNode(
    const std::vector<U8>& Bytes,
    ArchiveExtractionNode* Parent,
    U16 Index,
    U32 Offset,
    U32 Size,
    const std::string& Type,
    const std::string& Name,
    bool IsDirectory)
    : mBinaryReader{ Bytes }
    , mParent{ Parent }
    , mIndex{ Index }
    , mOffset{ Offset }
    , mSize{ Size }
    , mType{ Type }
    , mName{ Name }
    , mIsDirectory{ IsDirectory }
  {
    if (mIsDirectory)
    {
      ReadHeader();

      for (U32 i = 0; i < mEntries.size(); i++)
      {
        mBinaryReader.SeekAbsolute(mEntries[i].Offset);

        mNodes.emplace_back(new ArchiveExtractionNode
        {
          mBinaryReader.Bytes(mEntries[i].Size),
          this,
          (U16)i,
          mEntries[i].Offset,
          mEntries[i].Size,
          mEntries[i].Type,
          mEntries[i].Name,
          ValidateDirectory(mEntries[i]),
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

  void ArchiveExtractionNode::ExtractRecursive(U32 Count, fs::path File, ArchiveExtractionNode* Node, bool Verbose)
  {
    if (!Node)
    {
      Node = this;
    }

    if (Verbose)
    {
      for (U32 i = 0; i < Count; i++)
      {
        if ((i % 2 == 0) && (i >= 2)) LOG("|");
        else LOG(" ");
      }

      LOG("%05u @ %20s @ %4s\n", Node->mIndex, Node->mName.c_str(), Node->mType.c_str());
    }

    fs::path fileName = FsUtils::ToArchiveName(Node->mIndex, Node->mName, Node->mType);
    fs::path fileHeaderName = FsUtils::ToArchiveName(Node->mIndex, Node->mName, "HDR");
    fs::path dirHeaderName = FsUtils::ToArchiveName(-1, Node->mName, "HDR");

    fs::path fullFileName = File / fileName;
    fs::path fullFileHeaderName = File / fileHeaderName;
    fs::path fullDirHeaderName = File / fileName / dirHeaderName;

    if (Node->mIsDirectory)
    {
      if (Node->mType[0] == 0)
      {
        for (const auto& node : Node->mNodes)
        {
          ExtractRecursive(Count + 2, File, node, Verbose);
        }
      }
      else
      {
        if (Node->mParent)
        {
          FsUtils::CreateIfNotExists(fullFileName);

          FsUtils::WriteBinary(fullDirHeaderName, Node->mBinaryReader.Bytes(32, 0));
        }

        for (const auto& node : Node->mNodes)
        {
          ExtractRecursive(Count + 2, fullFileName, node, Verbose);
        }
      }
    }
    else
    {
      FsUtils::WriteBinary(fullFileName, Node->mBinaryReader.Bytes(Node->mBinaryReader.GetSize() - 32, 32));
      FsUtils::WriteBinary(fullFileHeaderName, Node->mBinaryReader.Bytes(32));
    }
  }

  void ArchiveExtractionNode::ReadHeader()
  {
    if (mParent)
    {
      mBinaryReader.SeekAbsolute(32);
    }
    else
    {
      mBinaryReader.SeekAbsolute(0);
    }

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
      if (mParent)
      {
        if (mType == "ROF")
        {
          mEntries[i].Offset -= 8;
        }
      }
      else
      {
        mEntries[i].Offset -= 32;
      }
    }

    for (U32 i = 0; i < mEntries.size(); i++)
    {
      mBinaryReader.SeekAbsolute(mEntries[i].Offset);
      mBinaryReader.SeekRelative(12);

      mEntries[i].Name = StringUtils::RemoveNulls(mBinaryReader.String(20));
    }

    for (U32 i = 1; i < mEntries.size(); i++)
    {
      mEntries[i - 1].Size = mEntries[i].Offset - mEntries[i - 1].Offset;
    }

    mEntries[mEntries.size() - 1].Size = (U32)mBinaryReader.GetSize() - mEntries[mEntries.size() - 1].Offset;
  }

  bool ArchiveExtractionNode::ValidateDirectory(const ArchiveEntry& Entry)
  {
    mBinaryReader.SeekAbsolute(Entry.Offset + 32);

    U32 size = mBinaryReader.Read<U32>();

    if (size > 0 && size < 65535)
    {
      for (U32 i = 0; i < size; i++)
      {
        if ((i * 4) >= ((U32)mBinaryReader.GetSize() - 4))
        {
          return false;
        }

        U32 offset = mBinaryReader.Read<U32>();

        if (offset >= mBinaryReader.GetSize())
        {
          return false;
        }
      }

      for (U32 i = 0; i < size; i++)
      {
        if ((i * 4) >= ((U32)mBinaryReader.GetSize() - 4))
        {
          return false;
        }

        std::string type = StringUtils::RemoveNulls(mBinaryReader.String(4));

        if (!sKnownDirectoryTypes.contains(type) && !sKnownFileTypes.contains(type))
        {
          return false;
        }
      }

      return true;
    }

    return false;
  }
}