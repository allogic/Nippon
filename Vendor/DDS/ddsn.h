
#pragma once

#include <inttypes.h>

#define DDSRESULT int
#define DDSBOOL int

#define DDS_FMT_UNKNOWN  0
#define DDS_FMT_R8G8B8A8 1
#define DDS_FMT_B8G8R8A8 2
#define DDS_FMT_B8G8R8X8 3
#define DDS_FMT_DXT1     50
#define DDS_FMT_DXT3     51
#define DDS_FMT_DXT5     52

#define DDS_CUBEMAP     0x00000001
#define DDS_VOLUME      0x00000002
#define DDS_CUBEMAP_PX  0x00000100
#define DDS_CUBEMAP_NX  0x00000200
#define DDS_CUBEMAP_PY  0x00000400
#define DDS_CUBEMAP_NY  0x00000800
#define DDS_CUBEMAP_PZ  0x00001000
#define DDS_CUBEMAP_NZ  0x00002000
#define DDS_CUBEMAP_FULL (DDS_CUBEMAP|DDS_CUBEMAP_PX|DDS_CUBEMAP_NX|DDS_CUBEMAP_PY|DDS_CUBEMAP_NY|DDS_CUBEMAP_PZ|DDS_CUBEMAP_NZ)
#define DDS_FILE_READER 0x10000000

#define DDS_SUCCESS 0
#define DDS_ENOTFND -1
#define DDS_EINVAL  -2
#define DDS_ENOTSUP -3


typedef unsigned char dds_byte;
typedef uint32_t dds_u32;


typedef struct _dds_image_info
{
	dds_u32 size;
	dds_u32 width;
	dds_u32 height;
	dds_u32 depth;
	dds_u32 pitch; /* or linear size */
	dds_u32 format;
}
dds_image_info;

typedef struct _dds_info
{
	/* reading state */
	void* data;
	int side, mip;
	size_t hdrsize;

	/* image info */
	dds_image_info image;
	dds_u32 mipcount;
	dds_u32 flags;
	size_t srcsize;
	dds_u32 sideoffsets[6];
	dds_u32 mipoffsets[16];
}
dds_info;


DDSRESULT dds_load_from_memory(dds_byte* bytes, size_t size, dds_info* out, const dds_u32* supfmt);
DDSRESULT dds_load_from_file(const char* file, dds_info* out, const dds_u32* supfmt);

DDSRESULT dds_seek(dds_info* info, int side, int mip);
void dds_getinfo(dds_info* info, dds_image_info* outplaneinfo);
DDSBOOL dds_read(dds_info* info, void* out);
dds_byte* dds_read_all(dds_info* info);

void dds_close(dds_info* info);
