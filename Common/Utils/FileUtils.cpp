#include <Common/Utils/FileUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<U8> FileUtils::ReadBinary(const std::string& File, U64 Size)
  {
    std::vector<U8> bytes;
    std::ifstream stream = std::ifstream{ File, std::ios::ate | std::ios::binary };

    if (stream.is_open())
    {
      if (Size == 0)
      {
        Size = stream.tellg();
      }
      else
      {
        Size = std::min(Size, (U64)stream.tellg());
      }

      bytes.resize(Size);

      stream.seekg(std::ios::beg);

      if (Size)
      {
        stream.read((char*)&bytes[0], Size);
      }

      stream.close();
    }

    return bytes;
  }

  std::string FileUtils::ReadText(const std::string& File, U64 Size)
  {
    std::string text;
    std::ifstream stream = std::ifstream{ File, std::ios::ate };

    if (stream.is_open())
    {
      if (Size == 0)
      {
        Size = stream.tellg();
      }
      else
      {
        Size = std::min(Size, (U64)stream.tellg());
      }

      text.resize(Size);

      stream.seekg(std::ios::beg);

      if (Size)
      {
        stream.read(&text[0], Size);
      }

      stream.close();
    }

    return text;
  }

  void FileUtils::WriteBinary(const std::string& File, const std::vector<U8>& Bytes, U64 Size)
  {
    std::ofstream stream = std::ofstream{ File, std::ios::binary };

    if (stream.is_open())
    {
      if (Size == 0)
      {
        Size = Bytes.size();
      }
      else
      {
        Size = std::max(Size, (U64)Bytes.size());
      }

      if (Size)
      {
        stream.write((char*)&Bytes[0], Size);
      }

      stream.close();
    }
  }

  void FileUtils::WriteText(const std::string& File, const std::string& Text, U64 Size)
  {
    std::ofstream stream = std::ofstream{ File };

    if (stream.is_open())
    {
      if (Size == 0)
      {
        Size = Text.size();
      }
      else
      {
        Size = std::max(Size, (U64)Text.size());
      }

      if (Size)
      {
        stream.write(&Text[0], Size);
      }

      stream.close();
    }
  }
}