#include <Archive/Converter/IndexConverter.h>

namespace Nippon
{
	std::vector<U32> IndexConverter::ScrToU32(std::vector<ScrVertex> const& Vertices)
	{
		std::vector<U32> indices = {};

		U16 vertexCount = (U16)Vertices.size();

		for (U16 i = 2; i < vertexCount; i++)
		{
			if (Vertices[i].Connection == 0x8000)
			{
				continue;
			}

			if (Vertices[i].Connection)
			{
				indices.emplace_back(i - 2);
				indices.emplace_back(i - 1);
				indices.emplace_back(i - 0);
			}
			else
			{
				indices.emplace_back(i - 1);
				indices.emplace_back(i - 2);
				indices.emplace_back(i - 0);
			}
		}

		return indices;
	}

	std::vector<U32> IndexConverter::MdToU32(std::vector<MdVertex> const& Vertices)
	{
		std::vector<U32> indices = {};

		U16 vertexCount = (U16)Vertices.size();

		for (U16 i = 2; i < vertexCount; i++)
		{
			if (Vertices[i].Connection == 0x8000)
			{
				continue;
			}

			if (Vertices[i].Connection)
			{
				indices.emplace_back(i - 2);
				indices.emplace_back(i - 1);
				indices.emplace_back(i - 0);
			}
			else
			{
				indices.emplace_back(i - 1);
				indices.emplace_back(i - 2);
				indices.emplace_back(i - 0);
			}
		}

		return indices;
	}
}