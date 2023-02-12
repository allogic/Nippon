#pragma once

#include <vector>
#include <string>
#include <cstring>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class BinaryReader
  {
  public:

    BinaryReader(const std::vector<U8>& Bytes) : mBytes{ Bytes } {}

  public:

    inline auto GetPosition() const { return mPosition; }
    inline auto GetSize() const { return mBytes.size(); }
    inline const auto& GetBytes() const { return mBytes; }

  public:

    inline void SeekRelative(I64 Value) { mPosition += Value; }
    inline void SeekAbsolute(I64 Value) { mPosition = Value; }

  public:

    template<typename T>
    T Read();

    template<typename T>
    std::vector<T> Read(U64 Count);

    template<typename T>
    void Read(std::vector<T>& Values, U64 Count);

    std::vector<U8> Bytes(U64 Count);
    std::string String(U64 Count);

  private:

    std::vector<U8> mBytes;

    U64 mPosition = 0;
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

    std::memcpy((U8*)&value, &mBytes[mPosition], sizeof(T));

    mPosition += sizeof(T);

    return value;
  }

  template<typename T>
  std::vector<T> BinaryReader::Read(U64 Count)
  {
    std::vector<T> values = {};

    values.resize(Count);

    if (Count > 0)
    {
      std::memcpy((U8*)&values[0], &mBytes[mPosition], Count * sizeof(T));
    }

    mPosition += Count * sizeof(T);

    return values;
  }

  template<typename T>
  void BinaryReader::Read(std::vector<T>& Values, U64 Count)
  {
    Values.resize(Count);

    if (Count > 0)
    {
      std::memcpy((U8*)&Values[0], &mBytes[mPosition], Count * sizeof(T));
    }

    mPosition += Count * sizeof(T);
  }
}