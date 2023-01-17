#pragma once

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Window
  {
  public:

    static inline R32 GetWidth() { return sWidth; }
    static inline R32 GetHeight() { return sHeight; }
    static inline R32V2 GetSize() { return { sWidth, sHeight }; }
    static inline R32 GetAspectRatio() { return sWidth / sHeight; }

  public:

    static inline void SetWidth(R32 Width) { sWidth = Width; }
    static inline void SetHeight(R32 Height) { sHeight = Height; }

  private:

    static inline R32 sWidth = 1920.0F;
    static inline R32 sHeight = 1080.0F;
  };
}