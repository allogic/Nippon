#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

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