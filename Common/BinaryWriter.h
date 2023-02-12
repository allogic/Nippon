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
  class BinaryWriter
  {
  public:

    inline auto GetPosition() const { return mPosition; }
    inline auto GetSize() const { return mBytes.size(); }
    inline const auto& GetBytes() const { return mBytes; }

  public:

    void SeekRelative(I64 Value);
    void SeekAbsolute(I64 Value);

  public:

    template<typename T>
    void Write(T Value);

    template<typename T>
    void Write(const std::vector<T>& Values);

    void String(const std::string& Value, U64 Count = 0);

  private:

    std::vector<U8> mBytes = {};
    U64 mPosition = 0;
  };
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  template<typename T>
  void BinaryWriter::Write(T Value)
  {
    if (mPosition >= mBytes.size())
    {
      mBytes.resize(mPosition + sizeof(T));
    }

    std::memcpy(&mBytes[mPosition], (U8*)&Value, sizeof(T));

    mPosition += sizeof(T);
  }

  template<typename T>
  void BinaryWriter::Write(const std::vector<T>& Values)
  {
    if (mPosition >= mBytes.size())
    {
      mBytes.resize(mPosition + (Values.size() * sizeof(T)));
    }

    if (Values.size())
    {
      std::memcpy(&mBytes[mPosition], (U8*)&Values[0], Values.size() * sizeof(T));
    }

    mPosition += Values.size() * sizeof(T);
  }
}