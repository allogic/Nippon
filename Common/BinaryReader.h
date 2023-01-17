#pragma once

#include <vector>
#include <string>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class BinaryReader
  {
  public:

    BinaryReader(const std::vector<U8>& Bytes);

  public:

    inline auto GetPosition() const { return mPosition; }

    inline auto GetSize() const { return mBytes.size(); }
    inline const auto& GetBytes() const { return mBytes; }

  public:

    template<typename T>
    T Read();

    template<typename T>
    std::vector<T> Read(U64 Count);

    auto Bytes(U64 Size)
    {
      std::vector<U8> bytes = {};

      if (mBytes.size() >= mPosition + Size)
      {
        bytes = { &mBytes[mPosition], &mBytes[mPosition + Size] };
      }

      mPosition += Size;

      return bytes;
    }
    auto String(U64 Size)
    {
      std::string string = {};

      if (mBytes.size() >= mPosition + Size)
      {
        string = { (char*)&mBytes[mPosition], (char*)&mBytes[mPosition + Size] };
      }

      mPosition += Size;

      return string;
    }

    void SeekRelative(U64 Offset)
    {
      mPosition += Offset;
    }
    void SeekAbsolute(U64 Offset)
    {
      mPosition = Offset;
    }

  private:

    const std::vector<U8> mBytes;

    U64 mPosition{};
  };
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  template<typename T>
  T BinaryReader::Read()
  {
    T value = {};

    if (mBytes.size() >= mPosition + sizeof(T))
    {
      value = *((T*)&mBytes[mPosition]);
    }

    mPosition += sizeof(T);

    return value;
  }

  template<typename T>
  std::vector<T> BinaryReader::Read(U64 Count)
  {
    std::vector<T> values = {};

    if (mBytes.size() >= mPosition + sizeof(T) * Count)
    {
      values = { &mBytes[mPosition], &mBytes[mPosition + sizeof(T) * Count]};
    }

    mPosition += sizeof(T) * Count;

    return values;
  }
}