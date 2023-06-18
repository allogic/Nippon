#include <Editor/Texture.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Texture2D::Texture2D(U32 Width, U32 Height, U32 Mips, TextureWrap Wrap, TextureFilter Filter)
    : mWidth{ Width }
    , mHeight{ Height }
    , mMips{ Mips }
    , mWrap{ Wrap }
    , mFilter{ Filter }
  {
    glGenTextures(1, &mTid);

    glBindTexture(GL_TEXTURE_2D, mTid);

    glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)mWrap);
    glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)mWrap);

    glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
    glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)mFilter);

    glTextureParameteri(mTid, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(mTid, GL_TEXTURE_MAX_LEVEL, (I32)(mMips - 1));

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Texture2D::~Texture2D()
  {
    glDeleteTextures(1, &mTid);
  }

  void Texture2D::Bind() const
  {
    glBindTexture(GL_TEXTURE_2D, mTid);
  }

  void Texture2D::Mount(U32 Index) const
  {
    glBindTextureUnit(Index, mTid);
  }

  void Texture2D::Unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  RenderTexture::RenderTexture(TextureWrap Wrap, TextureFilter Filter)
    : mWrap{ Wrap }
    , mFilter{ Filter }
  {
    glGenTextures(1, &mTid);

    glBindTexture(GL_TEXTURE_2D, mTid);

    glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)mWrap);
    glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)mWrap);

    glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
    glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)mFilter);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1, 1, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  RenderTexture::~RenderTexture()
  {
    glDeleteTextures(1, &mTid);
  }

  void RenderTexture::Bind() const
  {
    glBindTexture(GL_TEXTURE_2D, mTid);
  }

  void RenderTexture::Resize(U32 Width, U32 Height)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (I32)Width, (I32)Height, 0, GL_RGBA, GL_FLOAT, nullptr);
  }

  void RenderTexture::Mount(U32 Index) const
  {
    glBindTextureUnit(Index, mTid);
  }

  void RenderTexture::Unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  DepthStencilTexture::DepthStencilTexture(TextureWrap Wrap, TextureFilter Filter)
    : mWrap{ Wrap }
    , mFilter{ Filter }
  {
    glGenTextures(1, &mTid);

    glBindTexture(GL_TEXTURE_2D, mTid);

    glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)mWrap);
    glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)mWrap);

    glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
    glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)mFilter);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1, 1, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  DepthStencilTexture::~DepthStencilTexture()
  {
    glDeleteTextures(1, &mTid);
  }

  void DepthStencilTexture::Bind() const
  {
    glBindTexture(GL_TEXTURE_2D, mTid);
  }

  void DepthStencilTexture::Resize(U32 Width, U32 Height)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (I32)Width, (I32)Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
  }

  void DepthStencilTexture::Mount(U32 Index) const
  {
    glBindTextureUnit(Index, mTid);
  }

  void DepthStencilTexture::Unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}