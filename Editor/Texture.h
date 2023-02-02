#pragma once

#include <vector>

#include <Common/Types.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  enum TextureWrap
  {
    Repeat = GL_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
  };

  enum TextureFilter
  {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
  };

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  class Texture
  {
  public:

    Texture(U32 Width, U32 Height, TextureWrap Wrap, TextureFilter Filter, const std::vector<T>& Values = {});
    virtual ~Texture();

  public:

    inline auto GetId() const { return mTid; }

  public:

    void Resize(U32 Width, U32 Height);
    void Bind();
    void Mount(U32 Index);
    void UnBind();

  private:

    std::vector<T> mValues;

    U32 mTid = 0;
  };

  using Texture2DU8RGB = Texture<GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB8UI, U8>;
  using Texture2DU8RGBA = Texture<GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8UI, U8>;

  using Texture2DR32RGB = Texture<GL_TEXTURE_2D, GL_FLOAT, GL_RGB, GL_RGB32F, R32>;
  using Texture2DR32RGBA = Texture<GL_TEXTURE_2D, GL_FLOAT, GL_RGBA, GL_RGBA32F, R32>;

  using Texture2DU24U8DS = Texture<GL_TEXTURE_2D, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, U32>;
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  Texture<Type, Layout, Format, FormatInternal, T>::Texture(U32 Width, U32 Height, TextureWrap Wrap, TextureFilter Filter, const std::vector<T>& Values)
    : mValues{ Values }
  {
    glGenTextures(1, &mTid);

    glBindTexture(Type, mTid);

    glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)Wrap);
    glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)Wrap);

    glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)Filter);
    glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)Filter);

    T* pixels = (mValues.size()) ? &mValues[0] : nullptr;
    glTexImage2D(Type, 0, FormatInternal, Width, Height, 0, Format, Layout, pixels);

    glBindTexture(Type, 0);
  }

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  Texture<Type, Layout, Format, FormatInternal, T>::~Texture()
  {
    glDeleteTextures(1, &mTid);
  }

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  void Texture<Type, Layout, Format, FormatInternal, T>::Resize(U32 Width, U32 Height)
  {
    T* pixels = (mValues.size()) ? &mValues[0] : nullptr;
    glTexImage2D(Type, 0, FormatInternal, (I32)Width, (I32)Height, 0, Format, Layout, pixels);
  }

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  void Texture<Type, Layout, Format, FormatInternal, T>::Bind()
  {
    glBindTexture(Type, mTid);
  }

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  void Texture<Type, Layout, Format, FormatInternal, T>::Mount(U32 Index)
  {
    glBindTextureUnit(Index, mTid);
  }

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  void Texture<Type, Layout, Format, FormatInternal, T>::UnBind()
  {
    glBindTexture(Type, 0);
  }
}