#include <Common/Utils/TextureUtils.h>

#include <Editor/Texture.h>

#include <Vendor/DirectXTex/DirectXTex.h>

#ifdef APIENTRY
#undef APIENTRY
#endif

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Texture2D* TextureUtils::LoadDDS(const fs::path& File)
  {
    DirectX::ScratchImage image = {};

    DirectX::LoadFromDDSFile(File.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);

    U32 width = (U32)image.GetMetadata().width;
    U32 height = (U32)image.GetMetadata().height;
    U32 mips = 1;

    Texture2D* texture = new Texture2D{ width, height, mips, TextureWrap::ClampToEdge, TextureFilter::Linear };

    texture->Bind();

    for (U32 i = 0; i < mips; i++)
    {
      const DirectX::Image* mip = image.GetImage(i, 0, 0);

      glCompressedTexImage2D(GL_TEXTURE_2D, (I32)i, GL_COMPRESSED_RGBA_BPTC_UNORM, (I32)mip->width, (I32)mip->height, 0, (I32)(mip->width * mip->height), mip->pixels);
    }

    texture->Unbind();

    return texture;
  }

  Texture2D* TextureUtils::LoadDDS(const std::vector<U8>& Bytes)
  {
    DirectX::ScratchImage image = {};

    DirectX::LoadFromDDSMemory(&Bytes[0], Bytes.size(), DirectX::DDS_FLAGS_NONE, nullptr, image);

    U32 width = (U32)image.GetMetadata().width;
    U32 height = (U32)image.GetMetadata().height;
    U32 mips = (U32)image.GetMetadata().mipLevels - 2;

    Texture2D* texture = new Texture2D{ width, height, mips, TextureWrap::ClampToEdge, TextureFilter::Linear };

    texture->Bind();

    for (U32 i = 0; i < mips; i++)
    {
      const DirectX::Image* mip = image.GetImage(i, 0, 0);

      glCompressedTexImage2D(GL_TEXTURE_2D, (I32)i, GL_COMPRESSED_RGBA_BPTC_UNORM, (I32)mip->width, (I32)mip->height, 0, (I32)(mip->width * mip->height), mip->pixels);
    }

    texture->Unbind();

    return texture;
  }
}