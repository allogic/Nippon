#pragma once

#include <cstdio>

#ifdef NDEBUG
#define LOG(FMT, ...) (...)
#else
#define LOG(FMT, ...) std::printf(FMT, __VA_ARGS__)
#endif

#define ALIGN_DOWN(VALUE, ALIGNMENT) (VALUE & ~(ALIGNMENT - 1))
#define ALIGN_UP(VALUE, ALIGNMENT) ((VALUE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define LIT_TO_BIG_ENDIAN32(VALUE) \
	(((VALUE >> 24) & 0xFF) | \
	 ((VALUE >>  8) & 0xFF00) | \
	 ((VALUE <<  8) & 0xFF0000) | \
	 ((VALUE << 24) & 0xFF000000))

#define BIG_TO_LIT_ENDIAN32(VALUE) \
	(((VALUE >> 24) & 0xFF) | \
	 ((VALUE >>  8) & 0xFF00) | \
	 ((VALUE <<  8) & 0xFF0000) | \
	 ((VALUE << 24) & 0xFF000000))