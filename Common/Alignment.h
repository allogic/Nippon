#pragma once

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  template<unsigned P>
  class Align
  {
  public:

    template<typename T>
    constexpr static inline T Down(T Value) { return Value & (~(P - 1)); }

    template<typename T>
    constexpr static inline T Up(T Value) { return (Value + (P - 1)) & (~(P - 1)); }
  };
}