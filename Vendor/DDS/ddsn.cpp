

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ddsn.h"


typedef enum DXGI_FORMAT
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
	DXGI_FORMAT_FORCE_UINT = 0xffffffffUL
}
DXGI_FORMAT;


#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
				((dds_u32)(dds_byte)(ch0) | ((dds_u32)(dds_byte)(ch1) << 8) |       \
				((dds_u32)(dds_byte)(ch2) << 16) | ((dds_u32)(dds_byte)(ch3) << 24))
#endif /* defined(MAKEFOURCC) */


#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_LUMINANCEA  0x00020001  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA
#define DDS_PAL8        0x00000020  // DDPF_PALETTEINDEXED8

#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_DEPTH       0x00800000
#define DDSD_PITCH       0x00000008
#define DDSD_LINEARSIZE  0x00080000

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 // DDSCAPS_COMPLEX

#define DDSCAPS2_CUBEMAP 0x200
#define DDSCAPS2_CUBEMAP_POSITIVEX 0x400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x1000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x2000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x4000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x8000
#define DDSCAPS2_VOLUME 0x200000


typedef struct _DDS_PIXELFORMAT
{
	dds_u32  size;
	dds_u32  flags;
	dds_u32  fourCC;
	dds_u32  RGBBitCount;
	dds_u32  RBitMask;
	dds_u32  GBitMask;
	dds_u32  BBitMask;
	dds_u32  ABitMask;
}
DDS_PIXELFORMAT;

#define DDS_MAGIC "DDS "
typedef struct _DDS_HEADER
{
	char            magic[4];
	dds_u32         size;
	dds_u32         flags;
	dds_u32         height;
	dds_u32         width;
	dds_u32         pitchOrLinearSize;
	dds_u32         depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	dds_u32         mipMapCount;
	dds_u32         reserved1[11];
	DDS_PIXELFORMAT ddspf;
	dds_u32         caps;
	dds_u32         caps2;
	dds_u32         caps3;
	dds_u32         caps4;
	dds_u32         reserved2;
}
DDS_HEADER;

typedef struct _DDS_HEADER_DXT10
{
	DXGI_FORMAT dxgiFormat;
	dds_u32     resourceDimension;
	dds_u32     miscFlag; // see D3D11_RESOURCE_MISC_FLAG
	dds_u32     arraySize;
	dds_u32     reserved;
}
DDS_HEADER_DXT10;


#define ISBITMASK(r, g, b, a) (ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a)

static DXGI_FORMAT GetDXGIFormat(DDS_PIXELFORMAT ddpf)
{
	if (ddpf.flags & DDS_RGB)
	{
		// Note that sRGB formats are written using the "DX10" extended header

		switch (ddpf.RGBBitCount)
		{
		case 32:
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000)) return DXGI_FORMAT_R8G8B8A8_UNORM;
			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000)) return DXGI_FORMAT_B8G8R8A8_UNORM;
			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000)) return DXGI_FORMAT_B8G8R8X8_UNORM;

			// No DXGI format maps to ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000) aka D3DFMT_X8B8G8R8

			// Note that many common DDS reader/writers (including D3DX) swap the
			// the RED/BLUE masks for 10:10:10:2 formats. We assumme
			// below that the 'backwards' header mask is being used since it is most
			// likely written by D3DX. The more robust solution is to use the 'DX10'
			// header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

			// For 'correct' writers, this should be 0x000003ff, 0x000ffc00, 0x3ff00000 for RGB data
			if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000)) return DXGI_FORMAT_R10G10B10A2_UNORM;

			// No DXGI format maps to ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000) aka D3DFMT_A2R10G10B10

			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000)) return DXGI_FORMAT_R16G16_UNORM;

			if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
			{
				// Only 32-bit color channel format in D3D9 was R32F
				return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
			}
			break;

		case 24:
			// No 24bpp DXGI formats aka D3DFMT_R8G8B8
			break;

		case 16:
			if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000)) return DXGI_FORMAT_B5G5R5A1_UNORM;
			if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000)) return DXGI_FORMAT_B5G6R5_UNORM;

			// No DXGI format maps to ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x0000) aka D3DFMT_X1R5G5B5
			if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000)) return DXGI_FORMAT_B4G4R4A4_UNORM;

			// No DXGI format maps to ISBITMASK(0x0f00, 0x00f0, 0x000f, 0x0000) aka D3DFMT_X4R4G4B4

			// No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
			break;
		}
	}
	else if (ddpf.flags & DDS_LUMINANCE)
	{
		if (8 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000)) return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension

			// No DXGI format maps to ISBITMASK(0x0f, 0x00, 0x00, 0xf0) aka D3DFMT_A4L4
		}

		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000)) return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00)) return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
		}
	}
	else if (ddpf.flags & DDS_ALPHA)
	{
		if (8 == ddpf.RGBBitCount)
		{
			return DXGI_FORMAT_A8_UNORM;
		}
	}
	else if (ddpf.flags & DDS_FOURCC)
	{
		if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC) return DXGI_FORMAT_BC1_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC) return DXGI_FORMAT_BC2_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC) return DXGI_FORMAT_BC3_UNORM;

		// While pre-mulitplied alpha isn't directly supported by the DXGI formats,
		// they are basically the same as these BC formats so they can be mapped
		if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC) return DXGI_FORMAT_BC2_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC) return DXGI_FORMAT_BC3_UNORM;

		if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC) return DXGI_FORMAT_BC4_UNORM;
		if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC) return DXGI_FORMAT_BC4_UNORM;
		if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC) return DXGI_FORMAT_BC4_SNORM;

		if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC) return DXGI_FORMAT_BC5_UNORM;
		if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC) return DXGI_FORMAT_BC5_UNORM;
		if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC) return DXGI_FORMAT_BC5_SNORM;

		// BC6H and BC7 are written using the "DX10" extended header

		if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC) return DXGI_FORMAT_R8G8_B8G8_UNORM;
		if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC) return DXGI_FORMAT_G8R8_G8B8_UNORM;

		// Check for D3DFORMAT enums being set here
		switch (ddpf.fourCC)
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


static int dds_verify_header(dds_byte* bytes, size_t size, DDS_HEADER** outhdr, DDS_HEADER_DXT10** outhdr10)
{
	DDS_HEADER* header;

	if (memcmp(bytes, DDS_MAGIC, 4) != 0)
		return DDS_EINVAL;

	header = (DDS_HEADER*)bytes;
	*outhdr = header;
	*outhdr10 = NULL;

	if ((header->ddspf.flags & DDS_FOURCC) &&
		(MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
	{
		if (size < sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10))
			return DDS_EINVAL;
		*outhdr10 = (DDS_HEADER_DXT10*)(bytes + sizeof(DDS_HEADER));
	}

	return DDS_SUCCESS;
}

dds_u32 dds_supported_format(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM: return DDS_FMT_R8G8B8A8;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return DDS_FMT_B8G8R8A8;
	case DXGI_FORMAT_B8G8R8X8_UNORM: return DDS_FMT_B8G8R8X8;
	case DXGI_FORMAT_BC1_UNORM: return DDS_FMT_DXT1;
	case DXGI_FORMAT_BC2_UNORM: return DDS_FMT_DXT3;
	case DXGI_FORMAT_BC3_UNORM: return DDS_FMT_DXT5;
	default: return DDS_FMT_UNKNOWN;
	}
}

DDSRESULT dds_load_info(dds_info* out, size_t size, DDS_HEADER* hdr, DDS_HEADER_DXT10* hdr10)
{
	static const dds_u32 sideflags[6] = { DDS_CUBEMAP_PX, DDS_CUBEMAP_NX, DDS_CUBEMAP_PY, DDS_CUBEMAP_NY, DDS_CUBEMAP_PZ, DDS_CUBEMAP_NZ };

	int i;
	dds_u32 sum, sum2;
	dds_image_info plane;

	out->image.width = hdr->width;
	out->image.height = hdr->height;
	out->image.depth = hdr->flags & DDSD_DEPTH ? hdr->depth : 1;
	out->image.format = dds_supported_format(GetDXGIFormat(hdr->ddspf));

	out->mipcount = hdr->flags & DDSD_MIPMAPCOUNT ? hdr->mipMapCount : 1;
	out->flags = 0;
	if (hdr->caps2 & DDSCAPS2_VOLUME) out->flags |= DDS_VOLUME;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP) out->flags |= DDS_CUBEMAP;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP_POSITIVEX) out->flags |= DDS_CUBEMAP_PX;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) out->flags |= DDS_CUBEMAP_NX;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP_POSITIVEY) out->flags |= DDS_CUBEMAP_PY;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) out->flags |= DDS_CUBEMAP_NY;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) out->flags |= DDS_CUBEMAP_PZ;
	if (hdr->caps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ) out->flags |= DDS_CUBEMAP_NZ;

	if (out->mipcount > 16)
		return DDS_ENOTSUP;

	sum = 0;
	for (i = 0; i < out->mipcount; ++i)
	{
		out->mipoffsets[i] = sum;
		out->mip = i;
		dds_getinfo(out, &plane);
		sum += plane.size;
	}
	sum2 = 0;
	for (i = 0; i < 6; ++i)
	{
		out->sideoffsets[i] = sum2;
		if (out->flags & DDS_CUBEMAP && out->flags & sideflags[i])
			sum2 += sum;
	}

	out->image.size = out->flags & DDS_CUBEMAP ? sum2 : sum;

	out->data = NULL;
	out->side = 0;
	out->mip = 0;

	printf("loaded dds: w=%d h=%d d=%d fmt=%d mips=%d cube=%s size=%d\n", (int)out->image.width, (int)out->image.height,
		(int)out->image.depth, (int)out->image.format, (int)out->mipcount, out->flags & DDS_CUBEMAP ? "true" : "false", (int)out->image.size);

	return DDS_SUCCESS;
}

DDSRESULT dds_check_supfmt(dds_info* info, const dds_u32* supfmt)
{
	if (!supfmt)
		return DDS_SUCCESS;
	while (*supfmt)
	{
		if (info->image.format == *supfmt++)
			return DDS_SUCCESS;
	}
	return DDS_ENOTSUP;
}

DDSRESULT dds_load_from_memory(dds_byte* bytes, size_t size, dds_info* out, const dds_u32* supfmt)
{
	int res;
	DDS_HEADER* header;
	DDS_HEADER_DXT10* header10;

	if (size < sizeof(DDS_HEADER))
		return DDS_EINVAL;

	res = dds_verify_header(bytes, size, &header, &header10);
	if (res < 0)
		return res;

	if (dds_load_info(out, size, header, header10) < 0)
		return DDS_EINVAL;
	if (dds_check_supfmt(out, supfmt) < 0)
		return DDS_ENOTSUP;

	out->data = bytes;
	out->srcsize = size;
	out->hdrsize = 0;
	return DDS_SUCCESS;
}

DDSRESULT dds_load_from_file(const char* file, dds_info* out, const dds_u32* supfmt)
{
	int res;
	size_t numread, filesize;
	dds_byte ddsheader[sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10)];
	DDS_HEADER* header;
	DDS_HEADER_DXT10* header10;

	FILE* fp = fopen(file, "rb");
	if (!fp)
		return DDS_ENOTFND;

	numread = fread(ddsheader, 1, sizeof(DDS_HEADER), fp);
	if (numread < sizeof(DDS_HEADER))
	{
		fclose(fp);
		return DDS_EINVAL;
	}

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	res = dds_verify_header(ddsheader, numread, &header, &header10);
	if (res < 0)
	{
		fclose(fp);
		return res;
	}

	if (dds_load_info(out, filesize, header, header10) < 0)
	{
		fclose(fp);
		return DDS_EINVAL;
	}
	if (dds_check_supfmt(out, supfmt) < 0)
	{
		fclose(fp);
		return DDS_ENOTSUP;
	}

	out->data = fp;
	out->srcsize = filesize;
	out->hdrsize = sizeof(DDS_HEADER) + (header10 ? sizeof(DDS_HEADER_DXT10) : 0);
	out->flags |= DDS_FILE_READER;
	return DDS_SUCCESS;
}

DDSRESULT dds_seek(dds_info* info, int side, int mip)
{
	static const dds_u32 sideflags[6] = { DDS_CUBEMAP_PX, DDS_CUBEMAP_NX, DDS_CUBEMAP_PY, DDS_CUBEMAP_NY, DDS_CUBEMAP_PZ, DDS_CUBEMAP_NZ };

	if (mip < 0 || mip >= info->mipcount)
		return DDS_ENOTFND;

	if (info->flags & DDS_CUBEMAP)
	{
		if (side < 0 || side >= 6)
			return DDS_ENOTFND;
		if (!(info->flags & sideflags[side]))
			return DDS_ENOTFND;
	}
	else
	{
		if (side != 0)
			return DDS_ENOTFND;
	}

	info->side = side;
	info->mip = mip;
	return DDS_SUCCESS;
}

void dds_getinfo(dds_info* info, dds_image_info* plane)
{
	int mippwr = pow(2, info->mip);
	int fmt = info->image.format;

	*plane = info->image;
	plane->width /= mippwr;
	plane->height /= mippwr;
	plane->depth /= mippwr;
	if (plane->width < 1) plane->width = 1;
	if (plane->height < 1) plane->height = 1;
	if (plane->depth < 1) plane->depth = 1;

	if (fmt == DDS_FMT_DXT1 || fmt == DDS_FMT_DXT3 || fmt == DDS_FMT_DXT5)
	{
		int p = (plane->width + 3) / 4;
		if (p < 1) p = 1;
		plane->pitch = p * (fmt == DDS_FMT_DXT1 ? 8 : 16);
		plane->size = plane->pitch * ((plane->height + 3) / 4);
	}
	else /* RGBA8 / BGRA8 */
	{
		plane->pitch = plane->width * 4;
		plane->size = plane->pitch * plane->height * plane->depth;
	}
}

DDSBOOL dds_read(dds_info* info, void* out)
{
	dds_image_info plane;
	dds_u32 offset = info->hdrsize + info->sideoffsets[info->side] + info->mipoffsets[info->mip];
	dds_getinfo(info, &plane);

	if (info->flags & DDS_FILE_READER)
	{
		fseek((FILE*)info->data, offset, SEEK_SET);
		//		printf( "sideoff=%d mipoff=%d\n", info->sideoffsets[ info->side ], info->mipoffsets[ info->mip ] );
		return fread(out, 1, plane.size, (FILE*)info->data) == plane.size;
	}
	else
	{
		if (offset + plane.size > info->srcsize)
			return 0;
		memcpy(out, ((dds_byte*)info->data) + offset, plane.size);
		return 1;
	}
}

dds_byte* dds_read_all(dds_info* info)
{
	int s, m, nsz = info->flags & DDS_CUBEMAP ? 6 : 1;
	static const dds_u32 sideflags[6] = { DDS_CUBEMAP_PX, DDS_CUBEMAP_NX, DDS_CUBEMAP_PY, DDS_CUBEMAP_NY, DDS_CUBEMAP_PZ, DDS_CUBEMAP_NZ };
	dds_byte* out = (dds_byte*)malloc(info->image.size);

	for (s = 0; s < nsz; ++s)
	{
		if (nsz == 6 && !(info->flags & sideflags[s]))
			continue;
		for (m = 0; m < info->mipcount; ++m)
		{
			if (dds_seek(info, s, m) != DDS_SUCCESS ||
				!dds_read(info, out + info->sideoffsets[s] + info->mipoffsets[m]))
				goto fail;
			//			printf( "written s=%d m=%d at %d\n", s, m, info->sideoffsets[ s ] + info->mipoffsets[ m ] );
		}
	}

	return out;
fail:
	free(out);
	return NULL;
}

void dds_close(dds_info* info)
{
	if (info->flags & DDS_FILE_READER)
		fclose((FILE*)info->data);
	info->data = NULL;
}
