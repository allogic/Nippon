#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	enum TextureWrap
	{
		Repeat = 0x2901,
		ClampToEdge = 0x812F,
	};

	enum TextureFilter
	{
		Nearest = 0x2600,
		Linear = 0x2601,
	};

	class Texture2D
	{
	public:

		Texture2D(U32 Width, U32 Height, U32 Mips, TextureWrap Wrap, TextureFilter Filter);
		virtual ~Texture2D();

	public:

		inline auto GetId() const { return mTid; }
		inline auto GetWidth() const { return mWidth; }
		inline auto GetHeight() const { return mHeight; }
		inline auto GetMips() const { return mMips; }

	public:

		void Bind() const;
		void Mount(U32 Index) const;
		void UnMount() const;
		void Unbind() const;

	private:

		U32 mWidth;
		U32 mHeight;
		U32 mMips;
		TextureWrap mWrap;
		TextureFilter mFilter;

		U32 mTid = 0;
	};

	class RenderTexture
	{
	public:

		RenderTexture(TextureWrap Wrap, TextureFilter Filter);
		virtual ~RenderTexture();

	public:

		inline auto GetId() const { return mTid; }

	public:

		void Bind() const;
		void Resize(U32 Width, U32 Height);
		void Mount(U32 Index) const;
		void Unbind() const;

	private:

		TextureWrap mWrap;
		TextureFilter mFilter;

		U32 mTid = 0;
	};

	class DepthStencilTexture
	{
	public:

		DepthStencilTexture(TextureWrap Wrap, TextureFilter Filter);
		virtual ~DepthStencilTexture();

	public:

		inline auto GetId() const { return mTid; }

	public:

		void Bind() const;
		void Resize(U32 Width, U32 Height);
		void Mount(U32 Index) const;
		void Unbind() const;

	private:

		TextureWrap mWrap;
		TextureFilter mFilter;

		U32 mTid = 0;
	};
}