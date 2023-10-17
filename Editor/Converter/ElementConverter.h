#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ScrStructs.h>
#include <Editor/Structs/MdStructs.h>

namespace ark
{
	class ElementConverter
	{
	public:

		static std::vector<U32> ToElementBuffer(const std::vector<ScrVertex>& Vertices);
		static std::vector<U32> ToElementBuffer(const std::vector<MdVertex>& Vertices);
	};
}
