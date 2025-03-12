#include <Editor/DirectDrawSurface/DirectXTex.h>

#include <Editor/Glad/glad.h>

#include <Editor/Interface/Log.h>

#include <Editor/OpenGl/Texture2D.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Editor/StbImage/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Editor/StbImage/stb_image_write.h>

#include <Editor/Utility/TextureUtility.h>

/*
* OpenGL Extensions:
* ------------------
*  - GL_ARB_texture_compression_bptc
*  - GL_EXT_texture_compression_s3tc
*/

namespace Nippon
{
	namespace
	{
		U32 CreateTextureFromImage(const DirectX::Image* mip, DXGI_FORMAT format)
		{
			switch (format)
			{
				case DXGI_FORMAT_BC1_UNORM:
					return Texture2D::Create((U32)mip->width, (U32)mip->height, 1, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, GL_UNSIGNED_BYTE, mip->pixels, true);
				case DXGI_FORMAT_BC7_UNORM:
					return Texture2D::Create((U32)mip->width, (U32)mip->height, 1, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_UNSIGNED_BYTE, mip->pixels, true);
				default:
					Log::Add("Unsupported texture format 0x%X found\n", format);
					return 0;
			}
		}

		U32 LoadDDS(const DirectX::ScratchImage& image)
		{
			const DirectX::Image* mip = image.GetImage(0, 0, 0);
			
			return CreateTextureFromImage(mip, image.GetMetadata().format);
		}

		U32 LoadPNG(U8 const* data, I32 width, I32 height)
		{
			return Texture2D::Create(width, height, 4, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}

	U32 TextureUtility::ReadDDS(fs::path const& File)
	{
		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSFile(File.wstring().data(), DirectX::DDS_FLAGS_NONE, nullptr, image);

		return LoadDDS(image);
	}

	U32 TextureUtility::ReadDDS(U8 const* Bytes, U64 Size)
	{
		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSMemory(Bytes, Size, DirectX::DDS_FLAGS_NONE, nullptr, image);

		return LoadDDS(image);
	}

	U32 TextureUtility::ReadPNG(fs::path const& File)
	{
		I32 width = 0, height = 0, channels = 0;

		U8* data = stbi_load(File.string().data(), &width, &height, &channels, 0);

		U32 texture = LoadPNG(data, width, height);

		stbi_image_free(data);

		return texture;
	}

	U32 TextureUtility::ReadPNG(U8 const* Bytes, U64 Size)
	{
		I32 width = 0, height = 0, channels = 0;

		U8* data = stbi_load_from_memory(Bytes, (I32)Size, &width, &height, &channels, 0);

		U32 texture = LoadPNG(data, width, height);

		stbi_image_free(data);

		return texture;
	}

	void TextureUtility::WritePNG(U32 Id, fs::path const& File)
	{
		U32 width = Texture2D::GetWidth(Id);
		U32 height = Texture2D::GetHeight(Id);

		std::vector<U8> bytes = Texture2D::CopyRGBA(Id);

		stbi_flip_vertically_on_write(true);
		stbi_write_png(File.string().data(), width, height, 4, bytes.data(), 0);
	}

	std::vector<U8> TextureUtility::WritePNG(U32 Id)
	{
		U32 width = Texture2D::GetWidth(Id);
		U32 height = Texture2D::GetHeight(Id);

		std::vector<U8> bytes = Texture2D::CopyRGBA(Id);

		stbi_flip_vertically_on_write(true);

		I32 size = 0;
		U8* data = stbi_write_png_to_mem(bytes.data(), 0, width, height, 4, &size);

		std::vector<U8> result = { data, data + size };

		stbi_image_free(data);

		return result;
	}
}
