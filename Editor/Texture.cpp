#include <Editor/Texture.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	Texture2D::Texture2D(U32 Width, U32 Height, U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type)
		: mWidth{ Width }
		, mHeight{ Height }
		, mWrap{ Wrap }
		, mFilter{ Filter }
		, mFormat{ Format }
		, mFormatInternal{ FormatInternal }
		, mType{ Type }
	{
		glGenTextures(1, &mTid);

		glBindTexture(GL_TEXTURE_2D, mTid);

		glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)mWrap);
		glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)mWrap);

		glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
		glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)mFilter);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &mTid);
	}

	void Texture2D::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, mTid);
	}

	void Texture2D::Mount(U32 Index) const
	{
		glBindTextureUnit(Index, mTid);
	}

	void Texture2D::UnMount() const
	{
		glBindTextureUnit(0, mTid);
	}

	void Texture2D::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::vector<U8> Texture2D::Snapshot(U8 Channels) const
	{
		std::vector<U8> bytes = {};

		bytes.resize(mWidth * mHeight * Channels);

		glBindTexture(GL_TEXTURE_2D, mTid);
		glGetTexImage(GL_TEXTURE_2D, 0, mFormat, mType, &bytes[0]);
		glBindTexture(GL_TEXTURE_2D, 0);

		return bytes;
	}

	RenderTexture::RenderTexture(U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type)
		: mWidth{ 1 }
		, mHeight{ 1 }
		, mWrap{ Wrap }
		, mFilter{ Filter }
		, mFormat{ Format }
		, mFormatInternal{ FormatInternal }
		, mType{ Type }
	{
		glGenTextures(1, &mTid);

		glBindTexture(GL_TEXTURE_2D, mTid);

		glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)mWrap);
		glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)mWrap);

		glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
		glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)mFilter);

		glTexImage2D(GL_TEXTURE_2D, 0, mFormatInternal, 1, 1, 0, mFormat, mType, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	RenderTexture::~RenderTexture()
	{
		glDeleteTextures(1, &mTid);
	}

	void RenderTexture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, mTid);
	}

	void RenderTexture::Resize(U32 Width, U32 Height)
	{
		mWidth = Width;
		mHeight = Height;

		glTexImage2D(GL_TEXTURE_2D, 0, mFormatInternal, (I32)mWidth, (I32)mHeight, 0, mFormat, mType, nullptr);
	}

	void RenderTexture::Mount(U32 Index) const
	{
		glBindTextureUnit(Index, mTid);
	}

	void RenderTexture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	DepthStencilTexture::DepthStencilTexture(U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type)
		: mWidth{ 1 }
		, mHeight{ 1 }
		, mWrap{ Wrap }
		, mFilter{ Filter }
		, mFormat{ Format }
		, mFormatInternal{ FormatInternal }
		, mType{ Type }
	{
		glGenTextures(1, &mTid);

		glBindTexture(GL_TEXTURE_2D, mTid);

		glTextureParameteri(mTid, GL_TEXTURE_WRAP_S, (I32)mWrap);
		glTextureParameteri(mTid, GL_TEXTURE_WRAP_T, (I32)mWrap);

		glTextureParameteri(mTid, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
		glTextureParameteri(mTid, GL_TEXTURE_MAG_FILTER, (I32)mFilter);

		glTexImage2D(GL_TEXTURE_2D, 0, mFormatInternal, 1, 1, 0, mFormat, mType, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	DepthStencilTexture::~DepthStencilTexture()
	{
		glDeleteTextures(1, &mTid);
	}

	void DepthStencilTexture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, mTid);
	}

	void DepthStencilTexture::Resize(U32 Width, U32 Height)
	{
		mWidth = Width;
		mHeight = Height;

		glTexImage2D(GL_TEXTURE_2D, 0, mFormatInternal, (I32)mWidth, (I32)mHeight, 0, mFormat, mType, nullptr);
	}

	void DepthStencilTexture::Mount(U32 Index) const
	{
		glBindTextureUnit(Index, mTid);
	}

	void DepthStencilTexture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
