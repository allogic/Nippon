#pragma once

#include <vector>
#include <filesystem>
#include <cmath>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	struct DDS_PIXELFORMAT
	{
		U32 size;
		U32 flags;
		U32 fourCC;
		U32 RGBBitCount;
		U32 RBitMask;
		U32 GBitMask;
		U32 BBitMask;
		U32 ABitMask;
	};

	class DirectDrawSurface
	{
	public:

		DirectDrawSurface(const std::vector<U8>& Bytes);
		DirectDrawSurface(const fs::path& File);

	private:

		bool IsFormatValid() const;
		bool IsPalettized() const;
		bool IsCompressed() const;
		bool IsPacked() const;
		bool IsPlanar() const;

	private:

		bool DecodeHeader();
		bool CopyImage();

	private:

		U32 GetDXGIFormat(const DDS_PIXELFORMAT& PixelFormat) const;
		U32 BitsPerPixel() const;

	private:

		bool DetermineImageArray(U32& Images, U32& PixelSize) const;
		bool ComputePitch(U32 Width, U32 Height, U32& RowPitch, U32& SlicePitch) const;

	private:

		std::vector<U8> mBytes;

		U32 mConvFlags = 0;

		U32 mWidth = 0;
		U32 mHeight = 0;
		U32 mDepth = 0;
		U32 mDimension = 0;
		U32 mMipLevels = 0;
		U32 mArraySize = 0;
		U32 mFormat = 0;
		U32 mMiscFlags = 0;
		U32 mMiscFlags2 = 0;

		U32 mOffset = 0;
		U32 mSize = 0;

		U32* mPal8 = nullptr;
		U8* mPixels = nullptr;
	};
}