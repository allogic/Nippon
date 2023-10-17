#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

namespace ark
{
	class Texture2D
	{
	public:

		Texture2D(U32 Width, U32 Height, U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type);
		virtual ~Texture2D();

	public:

		inline const auto& GetId() const { return mTid; }
		inline const auto& GetWidth() const { return mWidth; }
		inline const auto& GetHeight() const { return mHeight; }
		inline const auto& GetWrap() const { return mWrap; };
		inline const auto& GetFilter() const { return mFilter; };
		inline const auto& GetFormat() const { return mFormat; };
		inline const auto& GetFormatInternal() const { return mFormatInternal; };
		inline const auto& GetType() const { return mType; };

	public:

		void Bind() const;
		void Mount(U32 Index) const;
		void UnMount() const;
		void UnBind() const;

	public:

		std::vector<U8> Snapshot(U8 Channels) const;

	private:

		U32 mWidth;
		U32 mHeight;
		U32 mWrap;
		U32 mFilter;
		U32 mFormat;
		U32 mFormatInternal;
		U32 mType;

		U32 mTid = 0;
	};

	class RenderTexture
	{
	public:

		RenderTexture(U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type);
		virtual ~RenderTexture();

	public:

		inline const auto& GetId() const { return mTid; }
		inline const auto& GetWidth() const { return mWidth; }
		inline const auto& GetHeight() const { return mHeight; }
		inline const auto& GetWrap() const { return mWrap; };
		inline const auto& GetFilter() const { return mFilter; };
		inline const auto& GetFormat() const { return mFormat; };
		inline const auto& GetFormatInternal() const { return mFormatInternal; };
		inline const auto& GetType() const { return mType; };

	public:

		void Bind() const;
		void Resize(U32 Width, U32 Height);
		void Mount(U32 Index) const;
		void UnBind() const;

	private:

		U32 mWidth;
		U32 mHeight;
		U32 mWrap;
		U32 mFilter;
		U32 mFormat;
		U32 mFormatInternal;
		U32 mType;

		U32 mTid = 0;
	};

	class DepthStencilTexture
	{
	public:

		DepthStencilTexture(U32 Wrap, U32 Filter, U32 Format, U32 FormatInternal, U32 Type);
		virtual ~DepthStencilTexture();

	public:

		inline const auto& GetId() const { return mTid; }
		inline const auto& GetWidth() const { return mWidth; }
		inline const auto& GetHeight() const { return mHeight; }
		inline const auto& GetWrap() const { return mWrap; };
		inline const auto& GetFilter() const { return mFilter; };
		inline const auto& GetFormat() const { return mFormat; };
		inline const auto& GetFormatInternal() const { return mFormatInternal; };
		inline const auto& GetType() const { return mType; };

	public:

		void Bind() const;
		void Resize(U32 Width, U32 Height);
		void Mount(U32 Index) const;
		void UnBind() const;

	private:

		U32 mWidth;
		U32 mHeight;
		U32 mWrap;
		U32 mFilter;
		U32 mFormat;
		U32 mFormatInternal;
		U32 mType;

		U32 mTid = 0;
	};
}
