#include <Editor/Texture2D.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	U32 Texture2D::Create(U32 Width, U32 Height, U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type, void* Data, bool Compressed)
	{
		U32 id = 0;

		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_2D, id);

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, (I32)Wrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, (I32)Wrap);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, (I32)Filter);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, (I32)Filter);

		if (Compressed)
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, FormatInternal, Width, Height, 0, Width * Height, Data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, FormatInternal, Width, Height, 0, Format, Type, Data);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		return id;
	}

	void Texture2D::Destroy(U32 Id)
	{
		glDeleteTextures(1, &Id);
	}

	void Texture2D::Destroy(const std::vector<U32>& Ids)
	{
		glDeleteTextures((I32)Ids.size(), Ids.data());
	}

	U32 Texture2D::GetWidth(U32 Id)
	{
		I32 value = 0;
		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_WIDTH, &value);
		return value;
	}

	U32 Texture2D::GetHeight(U32 Id)
	{
		I32 value = 0;
		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_HEIGHT, &value);
		return value;
	}

	U32 Texture2D::GetFormatInternal(U32 Id)
	{
		I32 value = 0;
		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_INTERNAL_FORMAT, &value);
		return value;
	}

	void Texture2D::Bind(U32 Id)
	{
		glBindTexture(GL_TEXTURE_2D, Id);
	}

	void Texture2D::Mount(U32 Index, U32 Id)
	{
		glBindTextureUnit(Index, Id);
	}

	void Texture2D::UnMount(U32 Index)
	{
		glBindTextureUnit(Index, 0);
	}

	void Texture2D::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::vector<U8> Texture2D::CopyRGB(U32 Id)
	{
		std::vector<U8> bytes = {};

		I32 width = 0;
		I32 height = 0;

		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_HEIGHT, &height);

		bytes.resize(width * height * 3);

		glBindTexture(GL_TEXTURE_2D, Id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes.data());
		glBindTexture(GL_TEXTURE_2D, 0);

		return bytes;
	}

	std::vector<U8> Texture2D::CopyRGBA(U32 Id)
	{
		std::vector<U8> bytes = {};

		I32 width = 0;
		I32 height = 0;

		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(Id, 0, GL_TEXTURE_HEIGHT, &height);

		bytes.resize(width * height * 4);

		glBindTexture(GL_TEXTURE_2D, Id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());
		glBindTexture(GL_TEXTURE_2D, 0);

		return bytes;
	}
}
