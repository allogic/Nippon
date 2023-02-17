#include <Common/Debug.h>
#include <Common/Alignment.h>

#include <Common/Utils/FileUtils.h>
#include <Common/Utils/StringUtils.h>

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
    mFileName = mFile.filename().string();
    mIsDirectory = fs::is_directory(mFile);

    StringUtils::FromArchiveName(mFileName, mIndex, mName, mType);

    if (mIsDirectory)
    {
      for (const auto& file : fs::directory_iterator{ mFile })
      {
        mNodes.emplace_back(new ArchiveCompressionNode{ file, this });
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
      
      if (mType == "AKT")
      {
        Node->mBinaryWriter.SeekRelative(80);

        for (U16 i = 0; i < (U16)Node->mNodes.size(); i++)
        {
          Node->mBinaryWriter.Write(Node->mNodes[i]->mBinaryWriter.GetBytes());

          if ((Node->mNodes.size() > 1) && (i < (Node->mNodes.size() - 1)))
          {
            Node->mBinaryWriter.SeekRelative(24);
          }
        }
      }
      else
      {
        for (U16 i = 0; i < (U16)Node->mNodes.size(); i++)
        {
          Node->mBinaryWriter.Write(Node->mNodes[i]->mBinaryWriter.GetBytes());
        }
      }
    }
    else
    {
      Node->mBinaryWriter.Write(FileUtils::ReadBinary(Node->mFile.string()));
    }
  }

  void ArchiveCompressionNode::WriteHeader()
  {
    mBinaryWriter.Write<U32>((U32)mNodes.size());
    
    U32 fileOffset = 4;
    
    fileOffset += (U32)mNodes.size() * 4;
    fileOffset += (U32)mNodes.size() * 4;

    fileOffset = Align<32>::Up(fileOffset);
    fileOffset += 32;

    //switch (fileOffset % 16)
    //{
    //  case 0: fileOffset += 8; break;
    //  case 4: fileOffset += 20; break;
    //  case 8: fileOffset += 16; break;
    //  case 12: fileOffset += 12; break;
    //}

    //fileOffset += (U32)GetMinOffsetByType();
    //fileOffset = (U32)GetAlignmentByType(fileOffset);
    
    for (const auto& node : mNodes)
    {
      mBinaryWriter.Write<U32>(fileOffset);
    
      fileOffset += (U32)node->mBinaryWriter.GetSize();

      if (node->mParent)
      {
        fileOffset += 32;
      }
    }

    for (const auto& node : mNodes)
    {
      mBinaryWriter.String(node->mType, 4);
    }

    mBinaryWriter.SeekAbsolute(Align<32>::Up(mBinaryWriter.GetPosition()));

    //U64 headerSize = mBinaryWriter.GetPosition();
    //
    //if (mParent)
    //{
    //  headerSize -= 24;
    //}
    //
    //switch (headerSize % 16)
    //{
    //  case 0: mBinaryWriter.SeekRelative(8); break;
    //  case 4: mBinaryWriter.SeekRelative(20); break;
    //  case 8: mBinaryWriter.SeekRelative(16); break;
    //  case 12: mBinaryWriter.SeekRelative(12); break;
    //}

    //mBinaryWriter.SeekAbsolute(GetAlignmentByType(mBinaryWriter.GetPosition()));
  }
}