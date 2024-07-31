#pragma once

#define ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((U64)VALUE) & ~(((U64)ALIGNMENT) - 1))
#define ALIGN_UP_BY(VALUE, ALIGNMENT) ((((U64)VALUE) + (((U64)ALIGNMENT) - 1)) & ~(((U64)ALIGNMENT) - 1))

#define ALIGN_PAGE_DOWN(VALUE) (((U64)VALUE) & ~((PAGE_SIZE) - 1))
#define ALIGN_PAGE_UP(VALUE) ((((U64)VALUE) + ((PAGE_SIZE) - 1)) & ~((PAGE_SIZE) - 1))

#define L2B_ENDIAN_16(VALUE) \
	((((VALUE) & 0xFF) << 8) | \
	 (((VALUE) & 0xFF00) >> 8))

#define B2L_ENDIAN_16(x) \
	((((VALUE) & 0xFF) << 8) | \
	 (((VALUE) & 0xFF00) >> 8))

#define L2B_ENDIAN_32(VALUE) \
	((((VALUE) & 0xFFUL) << 24) | \
	 (((VALUE) & 0xFF00UL) << 8) | \
	 (((VALUE) & 0xFF0000UL) >> 8) | \
	 (((VALUE) & 0xFF000000UL) >> 24))

#define B2L_ENDIAN_32(VALUE) \
	((((VALUE) & 0xFFUL) << 24) | \
	 (((VALUE) & 0xFF00UL) << 8) | \
	 (((VALUE) & 0xFF0000UL) >> 8) | \
	 (((VALUE) & 0xFF000000UL) >> 24))

#define L2B_ENDIAN_64(VALUE) \
	((((VALUE) & 0xFFULL) << 56) | \
	 (((VALUE) & 0xFF00ULL) << 40) | \
	 (((VALUE) & 0xFF0000ULL) << 24) | \
	 (((VALUE) & 0xFF000000ULL) << 8) | \
	 (((VALUE) & 0xFF00000000ULL) >> 8) | \
	 (((VALUE) & 0xFF0000000000ULL) >> 24) | \
	 (((VALUE) & 0xFF000000000000ULL) >> 40) | \
	 (((VALUE) & 0xFF00000000000000ULL) >> 56))

#define B2L_ENDIAN_64(VALUE) \
	((((VALUE) & 0xFFULL) << 56) | \
	 (((VALUE) & 0xFF00ULL) << 40) | \
	 (((VALUE) & 0xFF0000ULL) << 24) | \
	 (((VALUE) & 0xFF000000ULL) << 8) | \
	 (((VALUE) & 0xFF00000000ULL) >> 8) | \
	 (((VALUE) & 0xFF0000000000ULL) >> 24) | \
	 (((VALUE) & 0xFF000000000000ULL) >> 40) | \
	 (((VALUE) & 0xFF00000000000000ULL) >> 56))