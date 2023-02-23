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
    ArchiveCompressionNode* Parent)
    : mFile{ File }
    , mParent{ Parent }
  {
    mIsDirectory = fs::is_directory(mFile);

    ArchiveUtils::FromArchiveName(mFile.filename().string(), mIndex, mName, mType);

    if (mIsDirectory)
    {
      std::string metaName = ArchiveUtils::ToArchiveName(-1, "meta", "DIR");

      for (const auto& file : fs::directory_iterator{ mFile })
      {
        if (file.path().filename() != metaName)
        {
          mNodes.emplace_back(new ArchiveCompressionNode{ file, this });
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
    mBinaryWriter.Write<U32>((U32)mNodes.size());

    WriteMeta();
    WriteIndices();
    WriteTypes();

    if (mParent && mIsDirectory)
    {
      //mBinaryWriter.SeekRelative(32);
      mBinaryWriter.SeekRelative(mBinaryWriter.GetPosition() % 32);
    }

    mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));
  }

  void ArchiveCompressionNode::WriteFile()
  {
    mBinaryWriter.Write(FileUtils::ReadBinary(mFile));

    if (mType == "ROF")
    {
      mBinaryWriter.SeekAbsolute(Align<16>::Up(mBinaryWriter.GetPosition()));
    }
    else
    {
      mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));
    }
  }

  void ArchiveCompressionNode::WriteMeta()
  {
    if (mParent && mIsDirectory)
    {
      std::string metaName = ArchiveUtils::ToArchiveName(-1, "meta", "DIR");

      mBinaryWriter.Write(FileUtils::ReadBinary(mFile / metaName.c_str()));
    }
  }

  void ArchiveCompressionNode::WriteIndices()
  {
    U32 fileOffset = 4;

    fileOffset += (U32)mNodes.size() * 4;
    fileOffset += (U32)mNodes.size() * 4;

    if (mParent && mIsDirectory)
    {
      fileOffset += fileOffset % 32;
    }

    fileOffset = Align<32>::Up(fileOffset);
    fileOffset += 32;

    for (U16 i = 0; i < (U16)mNodes.size(); i++)
    {
      if (i < ((U16)mNodes.size() - 1))
      {
        mBinaryWriter.Write<U32>(fileOffset);
      }

      fileOffset += (U32)mNodes[i]->mBinaryWriter.GetSize();

      if (mNodes[i]->mParent && !mNodes[i]->mIsDirectory)
      {
        //fileOffset += 32;
      }
    }

    //fileOffset += 64;
    //fileOffset -= 512;

    mBinaryWriter.Write<U32>(fileOffset);
  }

  void ArchiveCompressionNode::WriteTypes()
  {
    for (const auto& node : mNodes)
    {
      mBinaryWriter.String(node->mType, 4);
    }
  }
}