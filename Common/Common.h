#pragma once

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  #pragma pack(push, 1)
  template<typename T>
  struct PackedTuple
  {
    T x;
    T y;
    T z;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  template<typename T>
  struct PackedPair
  {
    T x;
    T y;
  };
  #pragma pack(pop)
}