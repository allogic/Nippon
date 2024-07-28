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
	U32 TextureUtility::ReadDDS(fs::path const& File)
	{
		U32 texture = 0;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSFile(File.wstring().data(), DirectX::DDS_FLAGS_NONE, nullptr, image);

		DirectX::Image const* mip = image.GetImage(0, 0, 0);

		switch (image.GetMetadata().format)
		{
			case DXGI_FORMAT_BC1_UNORM: texture = Texture2D::Create((U32)mip->width, (U32)mip->height, 1, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, GL_UNSIGNED_BYTE, mip->pixels, true); break;
			case DXGI_FORMAT_BC7_UNORM: texture = Texture2D::Create((U32)mip->width, (U32)mip->height, 1, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_UNSIGNED_BYTE, mip->pixels, true); break;
			default:
			{
				Log::Add("Unsupported texture format 0x%X found\n", image.GetMetadata().format);
			}
		}

		return texture;
	}

	U32 TextureUtility::ReadDDS(U8 const* Bytes, U64 Size)
	{
		U32 texture = 0;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSMemory(Bytes, Size, DirectX::DDS_FLAGS_NONE, nullptr, image);

		DirectX::Image const* mip = image.GetImage(0, 0, 0);

		switch (image.GetMetadata().format)
		{
			case DXGI_FORMAT_BC1_UNORM: texture = Texture2D::Create((U32)mip->width, (U32)mip->height, 1, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGB, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, mip->pixels, true); break;
			case DXGI_FORMAT_BC7_UNORM: texture = Texture2D::Create((U32)mip->width, (U32)mip->height, 1, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_UNSIGNED_BYTE, mip->pixels, true); break;
			default:
			{
				Log::Add("Unsupported texture format 0x%X found\n", image.GetMetadata().format);
			}
		}

		return texture;
	}

	U32 TextureUtility::ReadPNG(fs::path const& File)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* data = stbi_load(File.string().data(), &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, 4, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		return texture;
	}

	U32 TextureUtility::ReadPNG(U8 const* Bytes, U64 Size)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* data = stbi_load_from_memory(Bytes, (I32)Size, &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, 4, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

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
