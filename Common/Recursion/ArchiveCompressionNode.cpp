#include <Common/Debug.h>
#include <Common/Alignment.h>

#include <Common/Utils/ArchiveUtils.h>
#include <Common/Utils/FileUtils.h>

#include <Common/Recursion/ArchiveCompressionNode.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  ArchiveCompressionNode::ArchiveCompressionNode(
    const fs::path& File,
    ArchiveCompressionNode* Parent,
    bool IsDat)
    : mFile{ File }
    , mParent{ Parent }
    , mIsDat{ IsDat }
  {
    mIsDirectory = fs::is_directory(mFile);

    ArchiveUtils::FromArchiveName(mFile.filename().string(), mIndex, mName, mType);

    if (mIsDirectory)
    {
      for (const auto& file : fs::directory_iterator{ mFile })
      {
        U16 subIndex = 0;
        std::string subName = "";
        std::string subType = "";

        ArchiveUtils::FromArchiveName(file.path().filename().string(), subIndex, subName, subType);

        if (subType != "HDR")
        {
          mNodes.emplace_back(new ArchiveCompressionNode{ file, this, mIsDat });
        }
      }
    }
  }

  ArchiveCompressionNode::~ArchiveCompressionNode()
  {
    for (auto& node : mNodes)
    {
      delete node;
      node = nullptr;
    }
  }

  void ArchiveCompressionNode::CompressRecursive(U32 Count, ArchiveCompressionNode* Node, bool Verbose)
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

    for (const auto& node : Node->mNodes)
    {
      CompressRecursive(Count + 2, node, Verbose);
    }

    if (Node->mIsDirectory)
    {
      Node->WriteHeader();
      
      for (U16 i = 0; i < (U16)Node->mNodes.size(); i++)
      {
        Node->mBinaryWriter.Write(Node->mNodes[i]->mBinaryWriter.GetBytes());
      }
    }
    else
    {
      Node->WriteFile();
    }
  }

  void ArchiveCompressionNode::WriteHeader()
  {
    if (mParent && mIsDirectory)
    {
      fs::path dirHeaderName = ArchiveUtils::ToArchiveName(-1, mName, "HDR");

      mBinaryWriter.Write(FileUtils::ReadBinary(mFile / dirHeaderName));
    }

    mBinaryWriter.Write<U32>((U32)mNodes.size());

    WriteIndices();
    WriteTypes();

    if (mParent && mIsDirectory)
    {
      if (mType == "AKT")
      {
        mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));
        mBinaryWriter.SeekRelative(40);
      }
      else if (mType == "TBL")
      {
        mBinaryWriter.SeekRelative(mBinaryWriter.GetPosition() % 16);
        mBinaryWriter.SeekAbsolute(Align<16>::Up(mBinaryWriter.GetPosition()));
      }
      else if (mType == "SCP")
      {
        mBinaryWriter.SeekRelative(mBinaryWriter.GetPosition() % 8);
        mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));
      }
      else
      {
        mBinaryWriter.SeekRelative(mBinaryWriter.GetPosition() % 16);
        mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));
      }
    }
    else if (!mParent && mIsDirectory)
    {
      if (mIsDat)
      {
        mBinaryWriter.SeekRelative(mBinaryWriter.GetPosition() % 8);
        mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));
      }
      else
      {
        mBinaryWriter.SeekRelative(mBinaryWriter.GetPosition() % 8);
        mBinaryWriter.SeekAbsolute(Align<16>::Up(mBinaryWriter.GetPosition()));
      }
    }
  }

  void ArchiveCompressionNode::WriteFile()
  {
    fs::path fileName = ArchiveUtils::ToArchiveName(mIndex, mName, mType);
    fs::path fileHeaderName = ArchiveUtils::ToArchiveName(mIndex, mName, "HDR");

    fs::path fullFileName = mFile.parent_path() / fileName;
    fs::path fullFileHeaderName = mFile.parent_path() / fileHeaderName;

    mBinaryWriter.Write(FileUtils::ReadBinary(fullFileHeaderName));
    mBinaryWriter.Write(FileUtils::ReadBinary(fullFileName));

    if (mType == "DDS")
    {

    }
    else if (mType == "ROF")
    {

    }
    else if (mType == "AK")
    {
      mBinaryWriter.SeekAbsolute(Align<8>::Up(mBinaryWriter.GetPosition()));
    }
    else
    {
      mBinaryWriter.SeekAbsolute(Align<16>::Up(mBinaryWriter.GetPosition()));
    }
  }

  void ArchiveCompressionNode::WriteIndices()
  {
    U32 fileOffset = 4;

    fileOffset += (U32)mNodes.size() * 4;
    fileOffset += (U32)mNodes.size() * 4;

    if (mParent && mIsDirectory)
    {
      if (mType == "AKT")
      {
        fileOffset = Align<32>::Up(fileOffset);
        fileOffset += 40;
      }
      else if (mType == "TBL")
      {
        fileOffset += fileOffset % 16;
        fileOffset = Align<16>::Up(fileOffset);
      }
      else if (mType == "SCP")
      {
        fileOffset += fileOffset % 8;
        fileOffset = Align<32>::Up(fileOffset);
      }
      else
      {
        fileOffset += fileOffset % 16;
        fileOffset = Align<32>::Up(fileOffset);
      }
    }
    else if (!mParent && mIsDirectory)
    {
      if (mIsDat)
      {
        fileOffset += fileOffset % 8;
        fileOffset = Align<32>::Up(fileOffset);
      }
      else
      {
        fileOffset += fileOffset % 8;
        fileOffset = Align<16>::Up(fileOffset);
      }
    }

    fileOffset += 32;

    for (const auto& node : mNodes)
    {
      mBinaryWriter.Write<U32>(fileOffset);

      fileOffset += (U32)node->mBinaryWriter.GetSize();
    }
  }

  void ArchiveCompressionNode::WriteTypes()
  {
    for (const auto& node : mNodes)
    {
      mBinaryWriter.String(node->mType, 4);
    }
  }
}