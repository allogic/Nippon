#include <Common/DirectDrawSurface.h>

#include <Common/Utils/TextureUtils.h>

#include <Editor/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/StbImage/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Vendor/StbImage/stb_image_write.h>

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
	Texture2D* TextureUtils::LoadDirectDrawSurface(const fs::path& File)
	{
		DirectX::ScratchImage image = {};

		//DirectDrawSurface dds = { File };

		DirectX::LoadFromDDSFile(File.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);

		U32 width = (U32)image.GetMetadata().width;
		U32 height = (U32)image.GetMetadata().height;
		U32 format = image.GetMetadata().format;
		U32 mips = 1;

		Texture2D* texture = new Texture2D{ width, height, mips, TextureWrap::Repeat, TextureFilter::Linear };

		if (format == DXGI_FORMAT_BC7_UNORM)
		{
			texture->Bind();

			for (U32 i = 0; i < mips; i++)
			{
				const DirectX::Image* mip = image.GetImage(i, 0, 0);

				glCompressedTexImage2D(GL_TEXTURE_2D, (I32)i, GL_COMPRESSED_RGBA_BPTC_UNORM, (I32)mip->width, (I32)mip->height, 0, (I32)(mip->width * mip->height), mip->pixels);
			}

			texture->UnBind();
		}

		return texture;
	}

	Texture2D* TextureUtils::LoadGeneric(const fs::path& File)
	{
		U32 width = 0;
		U32 height = 0;
		U32 channels = 0;

		U8* bytes = stbi_load(File.string().c_str(), (I32*)&width, (I32*)&height, (I32*)&channels, 0);

		Texture2D* texture = new Texture2D{ width, height, 1, TextureWrap::ClampToEdge, TextureFilter::Linear };

		texture->Bind();

		switch (channels)
		{
			case 3:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);

				break;
			}
			case 4:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

				break;
			}
		}

		texture->UnBind();

		stbi_image_free(bytes);

		return texture;
	}

	void TextureUtils::WriteJPG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File)
	{
		U32 channels = 3;
		U32 quality = 100;

		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(File.string().c_str(), Width, Height, channels, &Bytes[0], quality);
	}

	void TextureUtils::WritePNG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File)
	{
		U32 channels = 4;
		U32 stride = channels * Width;

		stride += (stride % 4) ? (4 - (stride % 4)) : 0;

		stbi_flip_vertically_on_write(true);
		stbi_write_png(File.string().c_str(), Width, Height, channels, &Bytes[0], stride);
	}
}