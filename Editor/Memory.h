#pragma once

#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class Memory
	{
	public:

		static void* Alloc(U64 Size, void const* Data = nullptr);
		static void Free(void* Pointer);

	public:

		static void CheckForLeaks();
	};
}