#include <Common/Utils/FsUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<U8> FsUtils::ReadBinary(const fs::path& File, U64 Size)
  {
    std::vector<U8> bytes = {};
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

  std::string FsUtils::ReadText(const fs::path& File, U64 Size)
  {
    std::string text = "";
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

  void FsUtils::WriteBinary(const fs::path& File, const std::vector<U8>& Bytes, U64 Size)
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

  void FsUtils::WriteText(const fs::path& File, const std::string& Text, U64 Size)
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

  void FsUtils::CreateIfNotExists(const fs::path& File)
  {
    if (!fs::exists(File))
    {
      fs::create_directory(File);
    }
  }

  void FsUtils::FromArchiveName(const std::string& String, U16& Index, std::string& Name, std::string& Type)
  {
    U64 indexOffset = String.find('@');
    U64 nameOffset = String.find('@', indexOffset + 1);

    Index = (U16)std::strtoul(String.substr(0, indexOffset).c_str(), nullptr, 10);
    Name = String.substr(indexOffset + 1, nameOffset - indexOffset - 1);
    Type = String.substr(nameOffset + 1);
  }

  std::string FsUtils::ToArchiveName(U16 Index, const std::string& Name, const std::string& Type)
  {
    std::string result = {};

    result.resize(5 + 2 + Name.size() + Type.size());

    std::snprintf(&result[0], 6, "%05u", Index);
    result[5] = '@';
    std::memcpy(&result[6], &Name[0], Name.size());
    result[6 + Name.size()] = '@';
    std::memcpy(&result[7 + Name.size()], &Type[0], Type.size());

    return result;
  }

  fs::path FsUtils::SearchFileByType(const fs::path& File, const std::string& Type)
  {
    for (const auto& file : fs::directory_iterator{ File })
    {
      U16 index = 0;
      std::string name = "";
      std::string type = "";

      FromArchiveName(file.path().stem().string(), index, name, type);

      if (type == Type)
      {
        return file.path();
      }
    }

    return "";
  }

  std::vector<fs::path> FsUtils::SearchFilesByTypeRecursive(const fs::path& File, const std::string& Type)
  {
    std::vector<fs::path> files = {};

    for (const auto& file : fs::recursive_directory_iterator{ File })
    {
      U16 index = 0;
      std::string name = "";
      std::string type = "";

      FromArchiveName(file.path().stem().string(), index, name, type);

      if (type == Type)
      {
        files.emplace_back(file);
      }
    }

    return files;
  }
}