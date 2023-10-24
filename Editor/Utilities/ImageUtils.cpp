#include <Editor/Texture2D.h>

#include <Editor/Utilities/ImageUtils.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/StbImage/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Vendor/StbImage/stb_image_write.h>

#include <Vendor/DirectXTex/DirectXTex.h>

#ifdef APIENTRY
#undef APIENTRY
#endif

#include <Editor/Glad/glad.h>

namespace ark
{
	U32 ImageUtils::ReadDDS(const fs::path& File)
	{
		U32 texture = 0;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSFile(File.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);

		const DirectX::Image* mip = image.GetImage(0, 0, 0);

		switch (image.GetMetadata().format)
		{
			case DXGI_FORMAT_BC7_UNORM:
			{
				texture = Texture2D::Create((U32)mip->width, (U32)mip->height, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, mip->pixels, true);
			}
		}

		return texture;
	}

    U32 ImageUtils::ReadDDS(U8* Bytes, U64 Size)
	{
		U32 texture = 0;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSMemory(Bytes, Size, DirectX::DDS_FLAGS_NONE, nullptr, image);

		const DirectX::Image* mip = image.GetImage(0, 0, 0);

		switch (image.GetMetadata().format)
		{
			case DXGI_FORMAT_BC7_UNORM:
			{
				texture = Texture2D::Create((U32)mip->width, (U32)mip->height, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, mip->pixels, true);
			}
		}

		return texture;
	}

    U32 ImageUtils::ReadPNG(const fs::path& File)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* data = stbi_load(File.string().c_str(), &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		return texture;
	}

	U32 ImageUtils::ReadPNG(U8* Bytes, U64 Size)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* data = stbi_load_from_memory(Bytes, (I32)Size, &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		return texture;
	}

	U32 ImageUtils::ReadJPG(const fs::path& File)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* data = stbi_load(File.string().c_str(), &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		return texture;
	}

	U32 ImageUtils::ReadJPG(U8* Bytes, U64 Size)
	{
		U32 texture = 0;

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* data = stbi_load_from_memory(Bytes, (I32)Size, &width, &height, &channels, 0);

		texture = Texture2D::Create(width, height, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		return texture;
	}

	void ImageUtils::WriteJPG(U32 Id, const fs::path& File)
	{
		U32 width = Texture2D::GetWidth(Id);
		U32 height = Texture2D::GetHeight(Id);

		std::vector<U8> bytes = Texture2D::CopyRGB(Id);

		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(File.string().c_str(), width, height, 3, bytes.data(), 100);
	}

	/*
	std::vector<U8> ImageUtils::WriteJPG(U32 Id)
	{
		U32 width = Texture2D::GetWidth(Id);
		U32 height = Texture2D::GetHeight(Id);

		std::vector<U8> bytes = Texture2D::CopyRGB(Id);

		stbi_flip_vertically_on_write(true);

		I32 size = 0;
		U8* data = stbi_write_jpg_to_mem(bytes.data(), 0, width, height, 3, &size);

		std::vector<U8> result = { data, data + size };

		stbi_image_free(data);

		return result;
	}
	*/

	void ImageUtils::WritePNG(U32 Id, const fs::path& File)
	{
		U32 width = Texture2D::GetWidth(Id);
		U32 height = Texture2D::GetHeight(Id);

		std::vector<U8> bytes = Texture2D::CopyRGBA(Id);

		stbi_flip_vertically_on_write(true);
		stbi_write_png(File.string().c_str(), width, height, 4, bytes.data(), 0);
	}

	std::vector<U8> ImageUtils::WritePNG(U32 Id)
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
