#include <Editor/Texture.h>
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
    Texture2D* TextureLoader::LoadDirectDrawSurface(const std::vector<U8>& Bytes)
	{
		Texture2D* texture = nullptr;

		DirectX::ScratchImage image = {};

		DirectX::LoadFromDDSMemory(Bytes.data(), Bytes.size(), DirectX::DDS_FLAGS_NONE, nullptr, image);

		U32 width = (U32)image.GetMetadata().width;
		U32 height = (U32)image.GetMetadata().height;
		U32 format = image.GetMetadata().format;

		switch (format)
		{
			case DXGI_FORMAT_BC7_UNORM:
			{
				texture = new Texture2D{ width, height, GL_REPEAT, GL_LINEAR, GL_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE };

				texture->Bind();

				const DirectX::Image* mip = image.GetImage(0, 0, 0);

				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_BPTC_UNORM, (I32)mip->width, (I32)mip->height, 0, (I32)(mip->width * mip->height), mip->pixels);

				texture->UnBind();
			}
		}

		return texture;
	}

    Texture2D* TextureLoader::LoadGeneric(const fs::path& File)
	{
		Texture2D* texture = nullptr;

		U32 width = 0;
		U32 height = 0;
		U32 channels = 0;

		U8* bytes = stbi_load(File.string().c_str(), (I32*)&width, (I32*)&height, (I32*)&channels, 0);

		switch (channels)
		{
			case 3:
			{
				texture = new Texture2D{ width, height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGB, GL_RGB32UI, GL_UNSIGNED_BYTE };

				texture->Bind();

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);

				texture->UnBind();

				break;
			}
			case 4:
			{
				texture = new Texture2D{ width, height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGBA, GL_RGBA32UI, GL_UNSIGNED_BYTE };

				texture->Bind();

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

				texture->UnBind();

				break;
			}
		}

		stbi_image_free(bytes);

		return texture;
	}
}
