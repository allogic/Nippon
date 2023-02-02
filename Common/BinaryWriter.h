#pragma once

#include <vector>
#include <string>

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

    template<typename T>
    void Write(T Value);

    template<typename T>
    void Write(const std::vector<T>& Values);

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
    mBytes.resize(mPosition + sizeof(T));

    for (U64 i = 0; i < sizeof(T); i++)
    {
      mBytes[mPosition + i] = ((U8*)&Value)[i];
    }

    mPosition += sizeof(T);
  }

  template<typename T>
  void BinaryWriter::Write(const std::vector<T>& Values)
  {
    mBytes.resize(mPosition + (Values.size() * sizeof(T)));

    for (const auto& value : Values)
    {
      for (U64 i = 0; i < sizeof(T); i++)
      {
        mBytes[mPosition + i] = ((U8*)&value)[i];
      }

      mPosition += sizeof(T);
    }
  }
}