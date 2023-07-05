#pragma once

#include <cstdio>
#include <cassert>

#include <Common/Platform.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

#if defined(OS_WINDOWS)
	#define LOG(FMT, ...) std::printf(FMT, __VA_ARGS__)
#elif defined(OS_LINUX)
	#define LOG(FMT, ...) std::printf(FMT __VA_OPT__(,) __VA_ARGS__)
#else
	#error "Platform not supported!"
#endif

#define ASSERT(EXPR) assert(EXPR)