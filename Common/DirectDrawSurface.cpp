#include <Common/Debug.h>

#include <Common/DirectDrawSurface.h>
#include <Common/BinaryReader.h>

#include <Common/Utils/FsUtils.h>

#define MAKEFOURCC(C0, C1, C2, C3) ((U32)(U8)(C0) | ((U32)(U8)(C1) << 8) | ((U32)(U8)(C2) << 16) | ((U32)(U8)(C3) << 24 ))
#define ISBITMASK(R, G, B, A) (PixelFormat.RBitMask == R && PixelFormat.GBitMask == G && PixelFormat.BBitMask == B && PixelFormat.ABitMask == A)

#define DDS_MAGIC 0x20534444

#define DDS_FOURCC    0x00000004
#define DDS_RGB       0x00000040
#define DDS_LUMINANCE 0x00020000
#define DDS_ALPHA     0x00000002
#define DDS_BUMPDUDV  0x00080000

#define DDS_HEIGHT 0x00000002
#define DDS_WIDTH  0x00000004

#define DDS_CUBEMAP 0x00000200

#define DDS_HEADER_FLAGS_TEXTURE    0x00001007
#define DDS_HEADER_FLAGS_MIPMAP     0x00020000
#define DDS_HEADER_FLAGS_VOLUME     0x00800000
#define DDS_HEADER_FLAGS_PITCH      0x00000008
#define DDS_HEADER_FLAGS_LINEARSIZE 0x00080000

#define DDS_CUBEMAP_POSITIVEX 0x00000600
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00
#define DDS_CUBEMAP_POSITIVEY 0x00001200
#define DDS_CUBEMAP_NEGATIVEY 0x00002200
#define DDS_CUBEMAP_POSITIVEZ 0x00004200
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200

#define DDS_CUBEMAP_ALLFACES (DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX | DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY | DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ)

#define XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT 116
#define XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT 117
#define XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT 118
#define XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS 119
#define XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT 120

#define WIN10_DXGI_FORMAT_P208 130
#define WIN10_DXGI_FORMAT_V208 131
#define WIN10_DXGI_FORMAT_V408 132

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	enum CONVERSION_FLAGS : U32
	{
		CONV_FLAGS_NONE = 0x0,
		CONV_FLAGS_EXPAND = 0x1,
		CONV_FLAGS_NOALPHA = 0x2,
		CONV_FLAGS_SWIZZLE = 0x4,
		CONV_FLAGS_PAL8 = 0x8,
		CONV_FLAGS_888 = 0x10,
		CONV_FLAGS_565 = 0x20,
		CONV_FLAGS_5551 = 0x40,
		CONV_FLAGS_4444 = 0x80,
		CONV_FLAGS_44 = 0x100,
		CONV_FLAGS_332 = 0x200,
		CONV_FLAGS_8332 = 0x400,
		CONV_FLAGS_A8P8 = 0x800,
		CONF_FLAGS_11ON12 = 0x1000,
		CONV_FLAGS_DX10 = 0x10000,
		CONV_FLAGS_PMALPHA = 0x20000,
		CONV_FLAGS_L8 = 0x40000,
		CONV_FLAGS_L16 = 0x80000,
		CONV_FLAGS_A8L8 = 0x100000,
	};

	enum DXGI_FORMAT : U32
	{
		DXGI_FORMAT_UNKNOWN = 0,
		DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
		DXGI_FORMAT_R32G32B32A32_UINT = 3,
		DXGI_FORMAT_R32G32B32A32_SINT = 4,
		DXGI_FORMAT_R32G32B32_TYPELESS = 5,
		DXGI_FORMAT_R32G32B32_FLOAT = 6,
		DXGI_FORMAT_R32G32B32_UINT = 7,
		DXGI_FORMAT_R32G32B32_SINT = 8,
		DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
		DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
		DXGI_FORMAT_R16G16B16A16_UNORM = 11,
		DXGI_FORMAT_R16G16B16A16_UINT = 12,
		DXGI_FORMAT_R16G16B16A16_SNORM = 13,
		DXGI_FORMAT_R16G16B16A16_SINT = 14,
		DXGI_FORMAT_R32G32_TYPELESS = 15,
		DXGI_FORMAT_R32G32_FLOAT = 16,
		DXGI_FORMAT_R32G32_UINT = 17,
		DXGI_FORMAT_R32G32_SINT = 18,
		DXGI_FORMAT_R32G8X24_TYPELESS = 19,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
		DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
		DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
		DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
		DXGI_FORMAT_R10G10B10A2_UNORM = 24,
		DXGI_FORMAT_R10G10B10A2_UINT = 25,
		DXGI_FORMAT_R11G11B10_FLOAT = 26,
		DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
		DXGI_FORMAT_R8G8B8A8_UNORM = 28,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
		DXGI_FORMAT_R8G8B8A8_UINT = 30,
		DXGI_FORMAT_R8G8B8A8_SNORM = 31,
		DXGI_FORMAT_R8G8B8A8_SINT = 32,
		DXGI_FORMAT_R16G16_TYPELESS = 33,
		DXGI_FORMAT_R16G16_FLOAT = 34,
		DXGI_FORMAT_R16G16_UNORM = 35,
		DXGI_FORMAT_R16G16_UINT = 36,
		DXGI_FORMAT_R16G16_SNORM = 37,
		DXGI_FORMAT_R16G16_SINT = 38,
		DXGI_FORMAT_R32_TYPELESS = 39,
		DXGI_FORMAT_D32_FLOAT = 40,
		DXGI_FORMAT_R32_FLOAT = 41,
		DXGI_FORMAT_R32_UINT = 42,
		DXGI_FORMAT_R32_SINT = 43,
		DXGI_FORMAT_R24G8_TYPELESS = 44,
		DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
		DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
		DXGI_FORMAT_R8G8_TYPELESS = 48,
		DXGI_FORMAT_R8G8_UNORM = 49,
		DXGI_FORMAT_R8G8_UINT = 50,
		DXGI_FORMAT_R8G8_SNORM = 51,
		DXGI_FORMAT_R8G8_SINT = 52,
		DXGI_FORMAT_R16_TYPELESS = 53,
		DXGI_FORMAT_R16_FLOAT = 54,
		DXGI_FORMAT_D16_UNORM = 55,
		DXGI_FORMAT_R16_UNORM = 56,
		DXGI_FORMAT_R16_UINT = 57,
		DXGI_FORMAT_R16_SNORM = 58,
		DXGI_FORMAT_R16_SINT = 59,
		DXGI_FORMAT_R8_TYPELESS = 60,
		DXGI_FORMAT_R8_UNORM = 61,
		DXGI_FORMAT_R8_UINT = 62,
		DXGI_FORMAT_R8_SNORM = 63,
		DXGI_FORMAT_R8_SINT = 64,
		DXGI_FORMAT_A8_UNORM = 65,
		DXGI_FORMAT_R1_UNORM = 66,
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
		DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
		DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
		DXGI_FORMAT_BC1_TYPELESS = 70,
		DXGI_FORMAT_BC1_UNORM = 71,
		DXGI_FORMAT_BC1_UNORM_SRGB = 72,
		DXGI_FORMAT_BC2_TYPELESS = 73,
		DXGI_FORMAT_BC2_UNORM = 74,
		DXGI_FORMAT_BC2_UNORM_SRGB = 75,
		DXGI_FORMAT_BC3_TYPELESS = 76,
		DXGI_FORMAT_BC3_UNORM = 77,
		DXGI_FORMAT_BC3_UNORM_SRGB = 78,
		DXGI_FORMAT_BC4_TYPELESS = 79,
		DXGI_FORMAT_BC4_UNORM = 80,
		DXGI_FORMAT_BC4_SNORM = 81,
		DXGI_FORMAT_BC5_TYPELESS = 82,
		DXGI_FORMAT_BC5_UNORM = 83,
		DXGI_FORMAT_BC5_SNORM = 84,
		DXGI_FORMAT_B5G6R5_UNORM = 85,
		DXGI_FORMAT_B5G5R5A1_UNORM = 86,
		DXGI_FORMAT_B8G8R8A8_UNORM = 87,
		DXGI_FORMAT_B8G8R8X8_UNORM = 88,
		DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
		DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
		DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
		DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
		DXGI_FORMAT_BC6H_TYPELESS = 94,
		DXGI_FORMAT_BC6H_UF16 = 95,
		DXGI_FORMAT_BC6H_SF16 = 96,
		DXGI_FORMAT_BC7_TYPELESS = 97,
		DXGI_FORMAT_BC7_UNORM = 98,
		DXGI_FORMAT_BC7_UNORM_SRGB = 99,
		DXGI_FORMAT_AYUV = 100,
		DXGI_FORMAT_Y410 = 101,
		DXGI_FORMAT_Y416 = 102,
		DXGI_FORMAT_NV12 = 103,
		DXGI_FORMAT_P010 = 104,
		DXGI_FORMAT_P016 = 105,
		DXGI_FORMAT_420_OPAQUE = 106,
		DXGI_FORMAT_YUY2 = 107,
		DXGI_FORMAT_Y210 = 108,
		DXGI_FORMAT_Y216 = 109,
		DXGI_FORMAT_NV11 = 110,
		DXGI_FORMAT_AI44 = 111,
		DXGI_FORMAT_IA44 = 112,
		DXGI_FORMAT_P8 = 113,
		DXGI_FORMAT_A8P8 = 114,
		DXGI_FORMAT_B4G4R4A4_UNORM = 115,
		DXGI_FORMAT_P208 = 130,
		DXGI_FORMAT_V208 = 131,
		DXGI_FORMAT_V408 = 132,
		DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
		DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
		DXGI_FORMAT_FORCE_UINT = 0xffffffff
	};

	enum TEX_MISC_FLAG : U32
	{
		TEX_MISC_TEXTURECUBE = 0x4,
	};

	enum DDS_RESOURCE_MISC_FLAG : U32
	{
		DDS_RESOURCE_MISC_TEXTURECUBE = 0x4,
	};

	enum DDS_RESOURCE_DIMENSION : U32
	{
		DDS_DIMENSION_TEXTURE1D = 2,
		DDS_DIMENSION_TEXTURE2D = 3,
		DDS_DIMENSION_TEXTURE3D = 4,
	};

	enum TEX_DIMENSION : U32
	{
		TEX_DIMENSION_TEXTURE1D = 2,
		TEX_DIMENSION_TEXTURE2D = 3,
		TEX_DIMENSION_TEXTURE3D = 4,
	};

	enum TEX_MISC_FLAG2 : U32
	{
		TEX_MISC2_ALPHA_MODE_MASK = 0x7,
	};

	enum DDS_MISC_FLAGS2 : U32
	{
		DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7,
	};

	enum TEX_ALPHA_MODE : U32
	{
		TEX_ALPHA_MODE_UNKNOWN = 0,
		TEX_ALPHA_MODE_STRAIGHT = 1,
		TEX_ALPHA_MODE_PREMULTIPLIED = 2,
		TEX_ALPHA_MODE_OPAQUE = 3,
		TEX_ALPHA_MODE_CUSTOM = 4,
	};

	enum DDS_ALPHA_MODE : U32
	{
		DDS_ALPHA_MODE_UNKNOWN = 0,
		DDS_ALPHA_MODE_STRAIGHT = 1,
		DDS_ALPHA_MODE_PREMULTIPLIED = 2,
		DDS_ALPHA_MODE_OPAQUE = 3,
		DDS_ALPHA_MODE_CUSTOM = 4,
	};

	struct DDS_HEADER
	{
		U32 size;
		U32 flags;
		U32 height;
		U32 width;
		U32 pitchOrLinearSize;
		U32 depth;
		U32 mipMapCount;
		U32 reserved1[11];
		DDS_PIXELFORMAT ddspf;
		U32 caps;
		U32 caps2;
		U32 caps3;
		U32 caps4;
		U32 reserved2;
	};

	struct DDS_HEADER_DXT10
	{
		DXGI_FORMAT dxgiFormat;
		U32 resourceDimension;
		U32 miscFlag;
		U32 arraySize;
		U32 miscFlags2;
	};

	DirectDrawSurface::DirectDrawSurface(const std::vector<U8>& Bytes)
		: mBytes{ Bytes }
	{
		if (DecodeHeader())
		{
			mOffset = sizeof(U32) + sizeof(DDS_HEADER);
			if (mConvFlags & CONV_FLAGS_DX10)
			{
				mOffset += sizeof(DDS_HEADER_DXT10);
			}

			if (mConvFlags & CONV_FLAGS_PAL8)
			{
				mPal8 = (U32*)(&mBytes[0] + mOffset);
				mOffset += (256 * sizeof(U32));
			}

			mSize = (U32)mBytes.size() - mOffset;
			mPixels = &mBytes[0] + mOffset;

			if (CopyImage())
			{
				LOG("Loaded\n");
			}
		}
	}

	DirectDrawSurface::DirectDrawSurface(const fs::path& File)
		: mBytes{ FsUtils::ReadBinary(File) }
	{
		if (DecodeHeader())
		{
			mOffset = sizeof(U32) + sizeof(DDS_HEADER);
			if (mConvFlags & CONV_FLAGS_DX10)
			{
				mOffset += sizeof(DDS_HEADER_DXT10);
			}

			if (mConvFlags & CONV_FLAGS_PAL8)
			{
				mPal8 = (U32*)(&mBytes[0] + mOffset);
				mOffset += (256 * sizeof(U32));
			}

			mSize = (U32)mBytes.size() - mOffset;
			mPixels = &mBytes[0] + mOffset;

			if (CopyImage())
			{
				LOG("Loaded\n");
			}
		}
	}

	bool DirectDrawSurface::IsFormatValid() const
	{
		return mFormat >= 1 && mFormat <= 191;
	}

	bool DirectDrawSurface::IsPalettized() const
	{
		switch (mFormat)
		{
			case DXGI_FORMAT_AI44:
			case DXGI_FORMAT_IA44:
			case DXGI_FORMAT_P8:
			case DXGI_FORMAT_A8P8:
				return true;
			default:
				return false;
		}
	}

	bool DirectDrawSurface::IsCompressed() const
	{
		switch (mFormat)
		{
			case DXGI_FORMAT_BC1_TYPELESS:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			case DXGI_FORMAT_BC2_TYPELESS:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			case DXGI_FORMAT_BC3_TYPELESS:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			case DXGI_FORMAT_BC4_TYPELESS:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
			case DXGI_FORMAT_BC5_TYPELESS:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
			case DXGI_FORMAT_BC6H_TYPELESS:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
			case DXGI_FORMAT_BC7_TYPELESS:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return true;
			default:
				return false;
		}
	}

	bool DirectDrawSurface::IsPacked() const
	{
		switch (mFormat)
		{
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
			case DXGI_FORMAT_YUY2:
			case DXGI_FORMAT_Y210:
			case DXGI_FORMAT_Y216:
				return true;
			default:
				return false;
		}
	}

	bool DirectDrawSurface::IsPlanar() const
	{
		switch (mFormat)
		{
			case DXGI_FORMAT_NV12:
			case DXGI_FORMAT_P010:
			case DXGI_FORMAT_P016:
			case DXGI_FORMAT_420_OPAQUE:
			case DXGI_FORMAT_NV11:
			case WIN10_DXGI_FORMAT_P208:
			case WIN10_DXGI_FORMAT_V208:
			case WIN10_DXGI_FORMAT_V408:
			case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
			case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
			case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
				return true;
			default:
				return false;
		}
	}

	bool DirectDrawSurface::DecodeHeader()
	{
		BinaryReader reader = { mBytes };

		if (reader.GetSize() < (sizeof(DDS_HEADER) + sizeof(U32)))
		{
			return false;
		}

		if (reader.Read<U32>() != DDS_MAGIC)
		{
			return false;
		}

		DDS_HEADER header = reader.Read<DDS_HEADER>();

		if (header.size != sizeof(DDS_HEADER) || header.ddspf.size != sizeof(DDS_PIXELFORMAT))
		{
			return false;
		}

		mMipLevels = header.mipMapCount;

		if (mMipLevels == 0)
		{
			mMipLevels = 1;
		}

		if ((header.ddspf.flags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourCC))
		{
			if (reader.GetSize() < (sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10)))
			{
				return false;
			}

			DDS_HEADER_DXT10 d3d10ext = reader.Read<DDS_HEADER_DXT10>();

			mConvFlags |= CONV_FLAGS_DX10;

			mArraySize = d3d10ext.arraySize;

			if (mArraySize == 0)
			{
				return false;
			}

			mFormat = d3d10ext.dxgiFormat;

			if (!IsFormatValid() || IsPalettized())
			{
				return false;
			}

			static_assert(TEX_MISC_TEXTURECUBE == DDS_RESOURCE_MISC_TEXTURECUBE, "DDS header mismatch");

			mMiscFlags = d3d10ext.miscFlag & ~TEX_MISC_TEXTURECUBE;

			switch (d3d10ext.resourceDimension)
			{
				case DDS_DIMENSION_TEXTURE1D:
				{
					if ((header.flags & DDS_HEIGHT) && header.height != 1)
					{
						return false;
					}

					mWidth = header.width;
					mHeight = 1;
					mDepth = 1;
					mDimension = TEX_DIMENSION_TEXTURE1D;

					break;
				}
				case DDS_DIMENSION_TEXTURE2D:
				{
					if (d3d10ext.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE)
					{
						mMiscFlags |= TEX_MISC_TEXTURECUBE;
						mArraySize *= 6;
					}

					mWidth = header.width;
					mHeight = header.height;
					mDepth = 1;
					mDimension = TEX_DIMENSION_TEXTURE2D;

					break;
				}
				case DDS_DIMENSION_TEXTURE3D:
				{
					if (!(header.flags & DDS_HEADER_FLAGS_VOLUME))
					{
						return false;
					}

					if (mArraySize > 1)
					{
						return false;
					}

					mWidth = header.width;
					mHeight = header.height;
					mDepth = header.depth;
					mDimension = TEX_DIMENSION_TEXTURE3D;

					break;
				}
				default:
				{
					return false;
				}
			}

			static_assert(static_cast<int>(TEX_MISC2_ALPHA_MODE_MASK) == static_cast<int>(DDS_MISC_FLAGS2_ALPHA_MODE_MASK), "DDS header mismatch");

			static_assert(static_cast<int>(TEX_ALPHA_MODE_UNKNOWN) == static_cast<int>(DDS_ALPHA_MODE_UNKNOWN), "DDS header mismatch");
			static_assert(static_cast<int>(TEX_ALPHA_MODE_STRAIGHT) == static_cast<int>(DDS_ALPHA_MODE_STRAIGHT), "DDS header mismatch");
			static_assert(static_cast<int>(TEX_ALPHA_MODE_PREMULTIPLIED) == static_cast<int>(DDS_ALPHA_MODE_PREMULTIPLIED), "DDS header mismatch");
			static_assert(static_cast<int>(TEX_ALPHA_MODE_OPAQUE) == static_cast<int>(DDS_ALPHA_MODE_OPAQUE), "DDS header mismatch");
			static_assert(static_cast<int>(TEX_ALPHA_MODE_CUSTOM) == static_cast<int>(DDS_ALPHA_MODE_CUSTOM), "DDS header mismatch");

			mMiscFlags2 = d3d10ext.miscFlags2;
		}
		else
		{
			mArraySize = 1;

			if (header.flags & DDS_HEADER_FLAGS_VOLUME)
			{
				mWidth = header.width;
				mHeight = header.height;
				mDepth = header.depth;
				mDimension = TEX_DIMENSION_TEXTURE3D;
			}
			else
			{
				if (header.caps2 & DDS_CUBEMAP)
				{
					if ((header.caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
					{
						return false;
					}

					mArraySize = 6;
					mMiscFlags |= TEX_MISC_TEXTURECUBE;
				}

				mWidth = header.width;
				mHeight = header.height;
				mDepth = 1;
				mDimension = TEX_DIMENSION_TEXTURE2D;
			}

			mFormat = GetDXGIFormat(header.ddspf);

			if (mFormat == DXGI_FORMAT_UNKNOWN)
			{
				return false;
			}
		}

		if (mConvFlags & CONV_FLAGS_PMALPHA)
		{
			mMiscFlags2 = (mMiscFlags2 & ~TEX_MISC2_ALPHA_MODE_MASK) | TEX_ALPHA_MODE_PREMULTIPLIED;
		}

		if (mWidth > 16384 || mHeight > 16384 || mMipLevels > 15)
		{
			return false;
		}

		if (mArraySize > 2048 || mDepth > 2048)
		{
			return false;
		}

		return true;
	}

	bool DirectDrawSurface::CopyImage()
	{
		//if (mSize == 0)
		//{
		//	return false;
		//}
		//
		//U32 images = 0;
		//U32 pixelSize = 0;
		//
		//if (!DetermineImageArray(images, pixelSize))
		//{
		//	return false;
		//}
		//
		//if ((images == 0) || (images != image.GetImageCount()))
		//{
		//	return E_FAIL;
		//}
		//
		//if (pixelSize > size)
		//{
		//	return HRESULT_E_HANDLE_EOF;
		//}
		//
		//std::unique_ptr<Image[]> timages(new (std::nothrow) Image[nimages]);
		//if (!timages)
		//{
		//	return E_OUTOFMEMORY;
		//}
		//
		//if (!SetupImageArray(
		//	const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(pPixels)),
		//	pixelSize,
		//	metadata,
		//	cpFlags,
		//	timages.get(),
		//	nimages))
		//{
		//	return E_FAIL;
		//}
		//
		//if (nimages != image.GetImageCount())
		//{
		//	return E_FAIL;
		//}
		//
		//const Image* images = image.GetImages();
		//if (!images)
		//{
		//	return E_FAIL;
		//}
		//
		//uint32_t tflags = (convFlags & CONV_FLAGS_NOALPHA) ? TEXP_SCANLINE_SETALPHA : 0u;
		//if (convFlags & CONV_FLAGS_SWIZZLE)
		//	tflags |= TEXP_SCANLINE_LEGACY;
		//
		//switch (metadata.dimension)
		//{
		//case TEX_DIMENSION_TEXTURE1D:
		//case TEX_DIMENSION_TEXTURE2D:
		//{
		//	size_t index = 0;
		//	for (size_t item = 0; item < metadata.arraySize; ++item)
		//	{
		//		size_t lastgood = 0;
		//		for (size_t level = 0; level < metadata.mipLevels; ++level, ++index)
		//		{
		//			if (index >= nimages)
		//				return E_FAIL;
		//
		//			if (images[index].height != timages[index].height)
		//				return E_FAIL;
		//
		//			size_t dpitch = images[index].rowPitch;
		//			const size_t spitch = timages[index].rowPitch;
		//
		//			const uint8_t* pSrc = timages[index].pixels;
		//			if (!pSrc)
		//				return E_POINTER;
		//
		//			uint8_t* pDest = images[index].pixels;
		//			if (!pDest)
		//				return E_POINTER;
		//
		//			if (IsCompressed(metadata.format))
		//			{
		//				size_t csize = std::min<size_t>(images[index].slicePitch, timages[index].slicePitch);
		//				memcpy(pDest, pSrc, csize);
		//
		//				if (cpFlags & CP_FLAGS_BAD_DXTN_TAILS)
		//				{
		//					if (images[index].width < 4 || images[index].height < 4)
		//					{
		//						csize = std::min<size_t>(images[index].slicePitch, timages[lastgood].slicePitch);
		//						memcpy(pDest, timages[lastgood].pixels, csize);
		//					}
		//					else
		//					{
		//						lastgood = index;
		//					}
		//				}
		//			}
		//			else if (IsPlanar(metadata.format))
		//			{
		//				const size_t count = ComputeScanlines(metadata.format, images[index].height);
		//				if (!count)
		//					return E_UNEXPECTED;
		//
		//				const size_t csize = std::min<size_t>(dpitch, spitch);
		//				for (size_t h = 0; h < count; ++h)
		//				{
		//					memcpy(pDest, pSrc, csize);
		//					pSrc += spitch;
		//					pDest += dpitch;
		//				}
		//			}
		//			else
		//			{
		//				for (size_t h = 0; h < images[index].height; ++h)
		//				{
		//					if (convFlags & CONV_FLAGS_EXPAND)
		//					{
		//						if (convFlags & CONV_FLAGS_4444)
		//						{
		//							if (!ExpandScanline(pDest, dpitch, DXGI_FORMAT_R8G8B8A8_UNORM,
		//								pSrc, spitch,
		//								(convFlags & CONF_FLAGS_11ON12) ? WIN11_DXGI_FORMAT_A4B4G4R4_UNORM : DXGI_FORMAT_B4G4R4A4_UNORM,
		//								tflags))
		//								return E_FAIL;
		//						}
		//						else if (convFlags & (CONV_FLAGS_565 | CONV_FLAGS_5551))
		//						{
		//							if (!ExpandScanline(pDest, dpitch, DXGI_FORMAT_R8G8B8A8_UNORM,
		//								pSrc, spitch,
		//								(convFlags & CONV_FLAGS_565) ? DXGI_FORMAT_B5G6R5_UNORM : DXGI_FORMAT_B5G5R5A1_UNORM,
		//								tflags))
		//								return E_FAIL;
		//						}
		//						else
		//						{
		//							const TEXP_LEGACY_FORMAT lformat = FindLegacyFormat(convFlags);
		//							if (!LegacyExpandScanline(pDest, dpitch, metadata.format,
		//								pSrc, spitch, lformat, pal8,
		//								tflags))
		//								return E_FAIL;
		//						}
		//					}
		//					else if (convFlags & CONV_FLAGS_SWIZZLE)
		//					{
		//						SwizzleScanline(pDest, dpitch, pSrc, spitch,
		//							metadata.format, tflags);
		//					}
		//					else
		//					{
		//						CopyScanline(pDest, dpitch, pSrc, spitch,
		//							metadata.format, tflags);
		//					}
		//
		//					pSrc += spitch;
		//					pDest += dpitch;
		//				}
		//			}
		//		}
		//	}
		//}
		//break;
		//
		//case TEX_DIMENSION_TEXTURE3D:
		//{
		//	size_t index = 0;
		//	size_t d = metadata.depth;
		//
		//	size_t lastgood = 0;
		//	for (size_t level = 0; level < metadata.mipLevels; ++level)
		//	{
		//		for (size_t slice = 0; slice < d; ++slice, ++index)
		//		{
		//			if (index >= nimages)
		//				return E_FAIL;
		//
		//			if (images[index].height != timages[index].height)
		//				return E_FAIL;
		//
		//			size_t dpitch = images[index].rowPitch;
		//			const size_t spitch = timages[index].rowPitch;
		//
		//			const uint8_t* pSrc = timages[index].pixels;
		//			if (!pSrc)
		//				return E_POINTER;
		//
		//			uint8_t* pDest = images[index].pixels;
		//			if (!pDest)
		//				return E_POINTER;
		//
		//			if (IsCompressed(metadata.format))
		//			{
		//				size_t csize = std::min<size_t>(images[index].slicePitch, timages[index].slicePitch);
		//				memcpy(pDest, pSrc, csize);
		//
		//				if (cpFlags & CP_FLAGS_BAD_DXTN_TAILS)
		//				{
		//					if (images[index].width < 4 || images[index].height < 4)
		//					{
		//						csize = std::min<size_t>(images[index].slicePitch, timages[lastgood + slice].slicePitch);
		//						memcpy(pDest, timages[lastgood + slice].pixels, csize);
		//					}
		//					else if (!slice)
		//					{
		//						lastgood = index;
		//					}
		//				}
		//			}
		//			else if (IsPlanar(metadata.format))
		//			{
		//				// Direct3D does not support any planar formats for Texture3D
		//				return HRESULT_E_NOT_SUPPORTED;
		//			}
		//			else
		//			{
		//				for (size_t h = 0; h < images[index].height; ++h)
		//				{
		//					if (convFlags & CONV_FLAGS_EXPAND)
		//					{
		//						if (convFlags & CONV_FLAGS_4444)
		//						{
		//							if (!ExpandScanline(pDest, dpitch, DXGI_FORMAT_R8G8B8A8_UNORM,
		//								pSrc, spitch,
		//								(convFlags & CONF_FLAGS_11ON12) ? WIN11_DXGI_FORMAT_A4B4G4R4_UNORM : DXGI_FORMAT_B4G4R4A4_UNORM,
		//								tflags))
		//								return E_FAIL;
		//						}
		//						else if (convFlags & (CONV_FLAGS_565 | CONV_FLAGS_5551))
		//						{
		//							if (!ExpandScanline(pDest, dpitch, DXGI_FORMAT_R8G8B8A8_UNORM,
		//								pSrc, spitch,
		//								(convFlags & CONV_FLAGS_565) ? DXGI_FORMAT_B5G6R5_UNORM : DXGI_FORMAT_B5G5R5A1_UNORM,
		//								tflags))
		//								return E_FAIL;
		//						}
		//						else
		//						{
		//							const TEXP_LEGACY_FORMAT lformat = FindLegacyFormat(convFlags);
		//							if (!LegacyExpandScanline(pDest, dpitch, metadata.format,
		//								pSrc, spitch, lformat, pal8,
		//								tflags))
		//								return E_FAIL;
		//						}
		//					}
		//					else if (convFlags & CONV_FLAGS_SWIZZLE)
		//					{
		//						SwizzleScanline(pDest, dpitch, pSrc, spitch, metadata.format, tflags);
		//					}
		//					else
		//					{
		//						CopyScanline(pDest, dpitch, pSrc, spitch, metadata.format, tflags);
		//					}
		//
		//					pSrc += spitch;
		//					pDest += dpitch;
		//				}
		//			}
		//		}
		//
		//		if (d > 1)
		//			d >>= 1;
		//	}
		//}
		//break;
		//
		//default:
		//	return E_FAIL;
		//}

		return true;
	}

	U32 DirectDrawSurface::GetDXGIFormat(const DDS_PIXELFORMAT& PixelFormat) const
	{
		if (PixelFormat.flags & DDS_RGB)
		{
			// Note that sRGB formats are written using the "DX10" extended header.

			switch (PixelFormat.RGBBitCount)
			{
				case 32:
					if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
					{
						return DXGI_FORMAT_R8G8B8A8_UNORM;
					}

					if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
					{
						return DXGI_FORMAT_B8G8R8A8_UNORM;
					}

					if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
					{
						return DXGI_FORMAT_B8G8R8X8_UNORM;
					}

					// No DXGI format maps to ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000) aka D3DFMT_X8B8G8R8

					// Note that many common DDS reader/writers (including D3DX) swap the
					// the RED/BLUE masks for 10:10:10:2 formats. We assumme
					// below that the 'backwards' header mask is being used since it is most
					// likely written by D3DX. The more robust solution is to use the 'DX10'
					// header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

					// For 'correct' writers, this should be 0x000003ff, 0x000ffc00, 0x3ff00000 for RGB data
					if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
					{
						return DXGI_FORMAT_R10G10B10A2_UNORM;
					}

					// No DXGI format maps to ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000) aka D3DFMT_A2R10G10B10

					if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
					{
						return DXGI_FORMAT_R16G16_UNORM;
					}

					if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
					{
						// Only 32-bit color channel format in D3D9 was R32F.
						return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114.
					}
					break;

				case 24:
					// No 24bpp DXGI formats aka D3DFMT_R8G8B8
					break;

				case 16:
					if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
					{
						return DXGI_FORMAT_B5G5R5A1_UNORM;
					}
					if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
					{
						return DXGI_FORMAT_B5G6R5_UNORM;
					}

					// No DXGI format maps to ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x0000) aka D3DFMT_X1R5G5B5.
					if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
					{
						return DXGI_FORMAT_B4G4R4A4_UNORM;
					}

					// No DXGI format maps to ISBITMASK(0x0f00, 0x00f0, 0x000f, 0x0000) aka D3DFMT_X4R4G4B4.

					// No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
					break;
			}
		}
		else if (PixelFormat.flags & DDS_LUMINANCE)
		{
			if (8 == PixelFormat.RGBBitCount)
			{
				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
				{
					return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
				}

				// No DXGI format maps to ISBITMASK(0x0f, 0x00, 0x00, 0xf0) aka D3DFMT_A4L4.
			}

			if (16 == PixelFormat.RGBBitCount)
			{
				if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
				{
					return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension.
				}
				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
				{
					return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension.
				}
			}
		}
		else if (PixelFormat.flags & DDS_ALPHA)
		{
			if (8 == PixelFormat.RGBBitCount)
			{
				return DXGI_FORMAT_A8_UNORM;
			}
		}
		else if (PixelFormat.flags & DDS_FOURCC)
		{
			if (MAKEFOURCC('D', 'X', 'T', '1') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC1_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '3') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC2_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '5') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC3_UNORM;
			}

			// While pre-mulitplied alpha isn't directly supported by the DXGI formats,
			// they are basically the same as these BC formats so they can be mapped
			if (MAKEFOURCC('D', 'X', 'T', '2') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC2_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '4') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC3_UNORM;
			}

			if (MAKEFOURCC('A', 'T', 'I', '1') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC4_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '4', 'U') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC4_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '4', 'S') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC4_SNORM;
			}

			if (MAKEFOURCC('A', 'T', 'I', '2') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC5_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '5', 'U') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC5_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '5', 'S') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC5_SNORM;
			}

			// BC6H and BC7 are written using the "DX10" extended header

			if (MAKEFOURCC('R', 'G', 'B', 'G') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_R8G8_B8G8_UNORM;
			}
			if (MAKEFOURCC('G', 'R', 'G', 'B') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_G8R8_G8B8_UNORM;
			}

			// Check for D3DFORMAT enums being set here.
			switch (PixelFormat.fourCC)
			{
				case 36: // D3DFMT_A16B16G16R16
					return DXGI_FORMAT_R16G16B16A16_UNORM;

				case 110: // D3DFMT_Q16W16V16U16
					return DXGI_FORMAT_R16G16B16A16_SNORM;

				case 111: // D3DFMT_R16F
					return DXGI_FORMAT_R16_FLOAT;

				case 112: // D3DFMT_G16R16F
					return DXGI_FORMAT_R16G16_FLOAT;

				case 113: // D3DFMT_A16B16G16R16F
					return DXGI_FORMAT_R16G16B16A16_FLOAT;

				case 114: // D3DFMT_R32F
					return DXGI_FORMAT_R32_FLOAT;

				case 115: // D3DFMT_G32R32F
					return DXGI_FORMAT_R32G32_FLOAT;

				case 116: // D3DFMT_A32B32G32R32F
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	U32 DirectDrawSurface::BitsPerPixel() const
	{
		switch (mFormat)
		{
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
				return 128;

			case DXGI_FORMAT_R32G32B32_TYPELESS:
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
				return 96;

			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R32G32_TYPELESS:
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
			case DXGI_FORMAT_R32G8X24_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			case DXGI_FORMAT_Y416:
			case DXGI_FORMAT_Y210:
			case DXGI_FORMAT_Y216:
				return 64;

			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			case DXGI_FORMAT_R10G10B10A2_UNORM:
			case DXGI_FORMAT_R10G10B10A2_UINT:
			case DXGI_FORMAT_R11G11B10_FLOAT:
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_R16G16_TYPELESS:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT:
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			case DXGI_FORMAT_B8G8R8X8_UNORM:
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			case DXGI_FORMAT_AYUV:
			case DXGI_FORMAT_Y410:
			case DXGI_FORMAT_YUY2:
				return 32;

			case DXGI_FORMAT_P010:
			case DXGI_FORMAT_P016:
			case DXGI_FORMAT_V408:
				return 24;

			case DXGI_FORMAT_R8G8_TYPELESS:
			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_SINT:
			case DXGI_FORMAT_R16_TYPELESS:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_D16_UNORM:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_SINT:
			case DXGI_FORMAT_B5G6R5_UNORM:
			case DXGI_FORMAT_B5G5R5A1_UNORM:
			case DXGI_FORMAT_A8P8:
			case DXGI_FORMAT_B4G4R4A4_UNORM:
			case DXGI_FORMAT_P208:
			case DXGI_FORMAT_V208:
				return 16;

			case DXGI_FORMAT_NV12:
			case DXGI_FORMAT_420_OPAQUE:
			case DXGI_FORMAT_NV11:
				return 12;

			case DXGI_FORMAT_R8_TYPELESS:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R8_SINT:
			case DXGI_FORMAT_A8_UNORM:
			case DXGI_FORMAT_BC2_TYPELESS:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			case DXGI_FORMAT_BC3_TYPELESS:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			case DXGI_FORMAT_BC5_TYPELESS:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
			case DXGI_FORMAT_BC6H_TYPELESS:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
			case DXGI_FORMAT_BC7_TYPELESS:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
			case DXGI_FORMAT_AI44:
			case DXGI_FORMAT_IA44:
			case DXGI_FORMAT_P8:
				return 8;

			case DXGI_FORMAT_R1_UNORM:
				return 1;

			case DXGI_FORMAT_BC1_TYPELESS:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			case DXGI_FORMAT_BC4_TYPELESS:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
				return 4;

			default:
				return 0;
		}
	}

	bool DirectDrawSurface::DetermineImageArray(U32& Images, U32& PixelSize) const
	{
		if (mWidth == 0 || mHeight == 0 || mDepth == 0)
		{
			return false;
		}

		if (mArraySize == 0)
		{
			return false;
		}

		if (mMipLevels == 0)
		{
			return false;
		}

		U32 totalPixelSize = 0;
		U32 nimages = 0;

		switch (mDimension)
		{
			case TEX_DIMENSION_TEXTURE1D:
			case TEX_DIMENSION_TEXTURE2D:
			{
				for (U32 item = 0; item < mArraySize; item++)
				{
					U32 w = mWidth;
					U32 h = mHeight;

					for (U32 level = 0; level < mMipLevels; level++)
					{
						U32 rowPitch = 0;
						U32 slicePitch = 0;

						if (ComputePitch(w, h, rowPitch, slicePitch))
						{
							totalPixelSize += slicePitch;
							++nimages;

							if (h > 1)
								h >>= 1;

							if (w > 1)
								w >>= 1;
						}
						else
						{
							Images = PixelSize = 0;

							return false;
						}
					}
				}

				break;
			}
			case TEX_DIMENSION_TEXTURE3D:
			{
				U32 w = mWidth;
				U32 h = mHeight;
				U32 d = mDepth;

				for (U32 level = 0; level < mMipLevels; level++)
				{
					U32 rowPitch = 0;
					U32 slicePitch = 0;

					if (ComputePitch(w, h, rowPitch, slicePitch))
					{
						for (U32 slice = 0; slice < d; slice++)
						{
							totalPixelSize += uint64_t(slicePitch);
							++nimages;
						}

						if (h > 1)
							h >>= 1;

						if (w > 1)
							w >>= 1;

						if (d > 1)
							d >>= 1;
					}
					else
					{
						Images = PixelSize = 0;

						return false;
					}
				}

				break;
			}
			default:
			{
				Images = PixelSize = 0;

				return false;
			}
		}

		Images = nimages;
		PixelSize = totalPixelSize;

		return true;
	}

	bool DirectDrawSurface::ComputePitch(U32 Width, U32 Height, U32& RowPitch, U32& SlicePitch) const
	{
		U32 pitch = 0;
		U32 slice = 0;

		switch (mFormat)
		{
			case DXGI_FORMAT_BC1_TYPELESS:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			case DXGI_FORMAT_BC4_TYPELESS:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
			{
				if (!IsCompressed())
				{
					return false;
				}

				U32 nbw = std::max<U32>(1, (mWidth + 3) / 4);
				U32 nbh = std::max<U32>(1, (mHeight + 3) / 4);
				pitch = nbw * 8;
				slice = pitch * nbh;

				break;
			}
			case DXGI_FORMAT_BC2_TYPELESS:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			case DXGI_FORMAT_BC3_TYPELESS:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			case DXGI_FORMAT_BC5_TYPELESS:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
			case DXGI_FORMAT_BC6H_TYPELESS:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
			case DXGI_FORMAT_BC7_TYPELESS:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
			{
				if (!IsCompressed())
				{
					return false;
				}

				U32 nbw = std::max<U32>(1, (mWidth + 3) / 4);
				U32 nbh = std::max<U32>(1, (mHeight + 3) / 4);
				pitch = nbw * 16;
				slice = pitch * nbh;

				break;
			}
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
			case DXGI_FORMAT_YUY2:
			{
				if (!IsPacked())
				{
					return false;
				}

				pitch = ((mWidth + 1) >> 1) * 4;
				slice = pitch * mHeight;

				break;
			}
			case DXGI_FORMAT_Y210:
			case DXGI_FORMAT_Y216:
			{
				if (!IsPacked())
				{
					return false;
				}

				pitch = ((mWidth + 1) >> 1) * 8;
				slice = pitch * mHeight;

				break;
			}
			case DXGI_FORMAT_NV12:
			case DXGI_FORMAT_420_OPAQUE:
			{
				if ((mHeight % 2) != 0)
				{
					return false;
				}

				if (!IsPlanar())
				{
					return false;
				}

				pitch = ((mWidth + 1) >> 1) * 2;
				slice = pitch * (mHeight + ((mHeight + 1) >> 1));

				break;
			}
			case DXGI_FORMAT_P010:
			case DXGI_FORMAT_P016:
			{
				if ((mHeight % 2) != 0)
				{
					return false;
				}
			}
			case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
			case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
			case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
			{
				if (!IsPlanar())
				{
					return false;
				}

				pitch = ((mWidth + 1) >> 1) * 4;
				slice = pitch * (mHeight + ((mHeight + 1) >> 1));

				break;
			}
			case DXGI_FORMAT_NV11:
			{
				if (!IsPlanar())
				{
					return false;
				}

				pitch = ((mWidth + 3) >> 2) * 4;
				slice = pitch * mHeight * 2;

				break;
			}
			case WIN10_DXGI_FORMAT_P208:
			{
				if (!IsPlanar())
				{
					return false;
				}

				pitch = ((mWidth + 1) >> 1) * 2;
				slice = pitch * mHeight * 2;

				break;
			}
			case WIN10_DXGI_FORMAT_V208:
			{
				if ((mHeight % 2) != 0)
				{
					return false;
				}

				if (!IsPlanar())
				{
					return false;
				}

				pitch = mWidth;
				slice = pitch * (mHeight + ((mHeight + 1) >> 1) * 2);

				break;
			}
			case WIN10_DXGI_FORMAT_V408:
			{
				if (!IsPlanar())
				{
					return false;
				}

				pitch = mWidth;
				slice = pitch * (mHeight + (mHeight >> 1) * 4);

				break;
			}
			default:
			{
				if (IsCompressed() || IsPacked() || IsPlanar())
				{
					return false;
				}

				U32 bpp = BitsPerPixel();

				if (!bpp)
				{
					return false;
				}

				pitch = (mWidth * bpp + 7) / 8;
				slice = pitch * mHeight;

				break;
			}
		}

		RowPitch = pitch;
		SlicePitch = slice;

		return true;
	}
}