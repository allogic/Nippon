#include <Glad/glad.h>

#include <OpenGl/Texture2D.h>

namespace Nippon
{
	U32 Texture2D::Create(U32 Width, U32 Height, U32 Channels, U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type, void const* Data, bool Compressed)
	{
		U32 id = 0;

		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_2D, id);

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, (I32)Wrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, (I32)Wrap);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, (I32)Filter);

		if (Compressed)
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, FormatInternal, Width, Height, 0, Width * Height * Channels, Data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, FormatInternal, Width, Height, 0, Format, Type, Data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return id;
	}

	void Texture2D::Destroy(U32 Texture)
	{
		glDeleteTextures(1, &Texture);
	}

	void Texture2D::Destroy(std::vector<U32> const& Textures)
	{
		glDeleteTextures((I32)Textures.size(), Textures.data());
	}

	U32 Texture2D::GetWidth(U32 Texture)
	{
		I32 width = 0;

		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_WIDTH, &width);

		return width;
	}

	U32 Texture2D::GetHeight(U32 Texture)
	{
		I32 height = 0;

		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_HEIGHT, &height);

		return height;
	}

	U32 Texture2D::GetFormatInternal(U32 Texture)
	{
		I32 format = 0;

		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

		return format;
	}

	void Texture2D::Bind(U32 Texture)
	{
		glBindTexture(GL_TEXTURE_2D, Texture);
	}

	void Texture2D::Mount(U32 Index, U32 Texture)
	{
		glBindTextureUnit(Index, Texture);
	}

	void Texture2D::UnMount(U32 Index)
	{
		glBindTextureUnit(Index, 0);
	}

	void Texture2D::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::vector<U8> Texture2D::CopyRGB(U32 Texture)
	{
		std::vector<U8> bytes = {};

		I32 width = 0;
		I32 height = 0;

		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_HEIGHT, &height);

		bytes.resize(width * height * 3);

		glBindTexture(GL_TEXTURE_2D, Texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes.data());
		glBindTexture(GL_TEXTURE_2D, 0);

		return bytes;
	}

	std::vector<U8> Texture2D::CopyRGBA(U32 Texture)
	{
		std::vector<U8> bytes = {};

		I32 width = 0;
		I32 height = 0;

		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(Texture, 0, GL_TEXTURE_HEIGHT, &height);

		bytes.resize(width * height * 4);

		glBindTexture(GL_TEXTURE_2D, Texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());
		glBindTexture(GL_TEXTURE_2D, 0);

		return bytes;
	}
}