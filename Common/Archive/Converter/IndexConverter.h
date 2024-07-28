#pragma once

#include <Common/Forward.h>
#include <Common/Types.h>

#include <Common/Archive/Structs/MeshStructs.h>

namespace Nippon
{
	class IndexConverter
	{
	public:

		static std::vector<U32> ScrToU32(std::vector<ScrVertex> const& Vertices);
		static std::vector<U32> MdToU32(std::vector<MdVertex> const& Vertices);
	};
}