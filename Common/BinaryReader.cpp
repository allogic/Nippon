#include <Common/BinaryReader.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<U8> BinaryReader::Bytes(U64 Count)
  {
    std::vector<U8> bytes = {};

    bytes.resize(Count);

    if (Count > 0)
    {
      std::memcpy(&bytes[0], &mBytes[mPosition], Count);
    }

    mPosition += Count;

    return bytes;
  }

  std::vector<U8> BinaryReader::Bytes(U64 Count, U64 Offset) const
  {
    std::vector<U8> bytes = {};

    bytes.resize(Count);

    if (Count > 0)
    {
      std::memcpy(&bytes[0], &mBytes[Offset], Count);
    }

    return bytes;
  }

  std::string BinaryReader::String(U64 Count)
  {
    std::string string = {};

    string.resize(Count);

    if (Count > 0)
    {
      std::memcpy(&string[0], &mBytes[mPosition], Count);
    }

    mPosition += Count;

    return string;
  }

  std::string BinaryReader::String(U64 Count, U64 Offset) const
  {
    std::string string = {};

    string.resize(Count);

    if (Count > 0)
    {
      std::memcpy(&string[0], &mBytes[Offset], Count);
    }

    return string;
  }
}