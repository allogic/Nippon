#include <Common/Crc32.h>

#include <Common/Trees/ArchiveNode.h>

#include <Common/Utils/DirUtils.h>
#include <Common/Utils/FileUtils.h>
#include <Common/Utils/StringUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  static const std::set<std::string> sKnownUnknownTypes
  {
    "AK",
    "CAM",
    "DDP", "DDS",
    "EFF", "EMD", "EST",
    "ICO",
    "MD", "MOT",
    "PSN",
    "ROF",
    "SEQ", "SES",
    "TBL",
  };

  static const std::set<std::string> sKnownItemTypes
  {
    "AK",
    "CAM",
    "DDP", "DDS",
    "EFF", "EMD", "EST",
    "ICO",
    "MD", "MOT",
    "ROF",
    "SEQ", "SES",
    "TBL",
  };

  static const std::set<std::string> sKnownCharacterTypes
  {
    "AK",
    "CAM",
    "DDP", "DDS",
    "EFF", "EMD", "EST",
    "ICO",
    "MD", "MOT",
    "ROF",
    "SEQ", "SES",
    "TBL",
  };

  static const std::set<std::string> sKnownWorldTypes
  {
    "A00", "A01", "ACT", "AK", "AKT", "ANS",
    "B00", "B01", "BIN", "BMH",
    "C00", "CAM", "CCH", "CMP",
    "D00", "DAT", "DDP", "DDS",
    "EAR", "ECT",  "EFF", "EFP", "EMD", "EST",
    "FI2", "FIS",
    "ICO", "IDD", "IDP", "ISL", "ITS",
    "JMP",
    "LI3",
    "MEH", "MOT", "MRT", "MSA", "MSD", "MSS",
    "RHT", "RNI", "ROF",
    "S00", "S01", "S02", "S03", "S04", "S05", "SCA", "SCI",  "SCL", "SCM", "SCP", "SCR", "SEH", "SEQ", "SSD", "SSL",
    "TAT", "TBL", "TRE", "TS", "TSC",
    "V00", "V01", "V02", "V03",
  };

  ArchiveNode::ArchiveNode(const std::vector<U8>& Bytes)
    : mBinaryReader{ Bytes }
    , mCrc32{ Crc32::FromBytes(Bytes) }
    , mIsArchive{ ContainsArchive() }
  {
    if (mIsArchive)
    {
      FetchHeader();

      for (U32 i = 0; i < mToc.size(); i++)
      {
        mBinaryReader.SeekAbsolute(mToc[i].Offset);

        auto it = mNodes.emplace(mToc[i].Type, new ArchiveNode{ mBinaryReader.Bytes(mToc[i].Size) });
        it->second->mOffset = mToc[i].Offset;
        it->second->mSize = mToc[i].Size;
        it->second->mType = mToc[i].Type;
        it->second->mName = mToc[i].Name;
      }
    }
  }

  ArchiveNode::~ArchiveNode()
  {
    for (auto& [type, node] : mNodes)
    {
      delete node;
      node = nullptr;
    }
  }

  void ArchiveNode::ExtractRecursive(const fs::path& File, ArchiveNode* Node)
  {
    if (!Node)
    {
      Node = this;
    }

    if (Node->IsArchive())
    {
      DirUtils::CreateIfNotExists(File / Node->GetType());

      for (const auto& [type, node] : *Node)
      {
        ExtractRecursive(File / Node->GetType(), node);
      }
    }
    else
    {
      std::string fileWithName = (File / ((Node->GetName() == "") ? std::to_string(Node->GetCrc32()) : Node->GetName())).string();
      std::string fileWithNameAndExtension = fileWithName + "." + Node->GetType();

      if (Node->GetSize())
      {
        if (fs::exists(fileWithNameAndExtension))
        {
          if (Node->GetSize() > fs::file_size(fileWithNameAndExtension))
          {
            FileUtils::WriteBinary(fileWithNameAndExtension, Node->GetBytes());
          }
        }
        else
        {
          FileUtils::WriteBinary(fileWithNameAndExtension, Node->GetBytes());
        }
      }
    }
  }

  void ArchiveNode::FetchHeader()
  {
    mBinaryReader.SeekAbsolute(0);

    mToc.resize(mBinaryReader.Read<U32>());

    for (U32 i = 0; i < mToc.size(); i++)
    {
      mToc[i].Offset = mBinaryReader.Read<U32>();
    }

    for (U32 i = 0; i < mToc.size(); i++)
    {
      mToc[i].Type = StringUtils::RemoveNulls(mBinaryReader.String(4));
    }

    for (U32 i = 0; i < mToc.size(); i++)
    {
      mBinaryReader.SeekAbsolute(mToc[i].Offset);
      mBinaryReader.SeekRelative(-20);

      mToc[i].Name = StringUtils::RemoveNulls(mBinaryReader.String(20));
    }

    for (U32 i = 1; i < mToc.size(); i++)
    {
      mToc[i - 1].Size = mToc[i].Offset - mToc[i - 1].Offset;
      mToc[i - 1].Size -= 24;
    }

    mToc[mToc.size() - 1].Size = ((U32)mBinaryReader.GetSize() - 1) - mToc[mToc.size() - 1].Offset;

    if (mToc[mToc.size() - 1].Size >= 24)
    {
      mToc[mToc.size() - 1].Size -= 24;
    }
    else
    {
      mToc[mToc.size() - 1].Size = 0;
    }
  }

  bool ArchiveNode::ContainsArchive()
  {
    mBinaryReader.SeekAbsolute(0);

    U32 size = mBinaryReader.Read<U32>();

    if (size > 0 && size < 4096)
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

        if (!sKnownWorldTypes.contains(type) &&
            !sKnownCharacterTypes.contains(type) &&
            !sKnownItemTypes.contains(type) &&
            !sKnownUnknownTypes.contains(type))
        {
          return false;
        }
      }

      return true;
    }

    return false;
  }
}