#pragma once

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  template<typename T>
  T Align4(T Value)
  {
    return (Value + 3) & (~T(3));
  }

  template<typename T>
  T Align8(T Value)
  {
    return (Value + 7) & (~T(7));
  }

  template<typename T>
  T Align16(T Value)
  {
    return (Value + 15) & (~T(15));
  }

  template<typename T>
  T Align64(T Value)
  {
    return (Value + 63) & (~T(63));
  }
}