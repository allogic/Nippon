#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <algorithm>

int CreateDDSTextureFromMemory(const char unsigned* ddsData, size_t ddsDataSize, size_t& width, size_t& height, char unsigned** pixelBuffer);