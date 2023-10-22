#include <Editor/Texture2D.h>
#include <Editor/TextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/StbImage/stb_image.h>

#include <Vendor/DirectXTex/DirectXTex.h>

#ifdef APIENTRY
#undef APIENTRY
#endif

#include <Editor/Glad/glad.h>

namespace ark
{
	U32 TextureLoader::LoadDDS(const fs::path& File)
	{
		U32 texture = 0;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSFile(File.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);

		const DirectX::Image* mip = image.GetImage(0, 0, 0);

		switch (image.GetMetadata().format)
		{
			case DXGI_FORMAT_BC7_UNORM:
			{
				texture = Texture2D::Create((U32)mip->width, (U32)mip->height, GL_REPEAT, GL_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, mip->pixels, true);
			}
		}

		return texture;
	}

    U32 TextureLoader::LoadDDS(U8* Bytes, U64 Size)
	{
		U32 texture = 0;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSMemory(Bytes, Size, DirectX::DDS_FLAGS_NONE, nullptr, image);

		const DirectX::Image* mip = image.GetImage(0, 0, 0);

		switch (image.GetMetadata().format)
		{
			case DXGI_FORMAT_BC7_UNORM:
			{
				texture = Texture2D::Create((U32)mip->width, (U32)mip->height, GL_REPEAT, GL_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, mip->pixels, true);
			}
		}

		return texture;
	}

    U32 TextureLoader::LoadPNG(const fs::path& File)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* bytes = stbi_load(File.string().c_str(), &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

		stbi_image_free(bytes);

		return texture;
	}

	U32 TextureLoader::LoadPNG(U8* Bytes, U64 Size)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* bytes = stbi_load_from_memory(Bytes, (I32)Size, &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

		stbi_image_free(bytes);

		return texture;
	}

	U32 TextureLoader::LoadJPG(const fs::path& File)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* bytes = stbi_load(File.string().c_str(), &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, bytes);

		stbi_image_free(bytes);

		return texture;
	}

	U32 TextureLoader::LoadJPG(U8* Bytes, U64 Size)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* bytes = stbi_load_from_memory(Bytes, (I32)Size, &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, bytes);

		stbi_image_free(bytes);

		return texture;
	}
}
