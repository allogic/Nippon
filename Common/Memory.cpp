#include <Common/Memory.h>

namespace Nippon
{
	static U64 sAllocated = 0;

	void* Memory::Alloc(U64 Size, void const* Data)
	{
		U64 size = sizeof(U64) + Size;
		U64* block = (U64*)std::malloc(size);

		if (block)
		{
			sAllocated += size;
			*block = size;
			block += 1;

			if (Data)
			{
				std::memcpy(block, Data, Size);
			}
		}

		return block;
	}

	void Memory::Free(void* Pointer)
	{
		if (Pointer)
		{
			U64* block = ((U64*)Pointer) - 1;
			sAllocated -= *block;
			std::free(block);
		}
	}

	void Memory::CheckForLeaks()
	{
		assert(sAllocated == 0);
	}
}