#pragma once

#include <Common/Types.h>

#include <Editor/Texture.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class FrameBuffer
  {
  public:

    FrameBuffer();
    virtual ~FrameBuffer();

  public:

    inline auto GetId() const { return mFbo; }
    inline const auto& GetColorTexture() const { return mColorTexture; }

  public:

    void Resize(U32 Width, U32 Height);

  private:

    U32 mFbo = 0;

    Texture2DR32RGBA mColorTexture = { 1, 1, ClampToEdge, Linear };
    Texture2DU24U8DS mDepthStencilTexture = { 1, 1, ClampToEdge, Linear };
  };
}