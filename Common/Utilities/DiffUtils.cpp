#include <Common/Macros.h>

#include <Common/Utilities/DiffUtils.h>

namespace ark
{
	bool DiffUtils::Compare(const std::vector<U8>& Left, const std::vector<U8>& Right, std::vector<std::pair<U64, U64>>& Indices)
	{
		U64 size = std::min(Left.size(), Right.size());

		U64 leftIndex = 0;
		U64 rightIndex = 0;

		while ((leftIndex < size) && ((leftIndex + 1) < size) && (rightIndex < size) && ((rightIndex + 1) < size))
		{
			if (Left[leftIndex] != Right[rightIndex])
			{
				Indices.emplace_back(leftIndex, rightIndex);

				U64 offset = 0;

				// TODO: Implement clever window compare search..

				while (Left[leftIndex + offset] != Right[rightIndex + offset])
				{
					offset += 1;

					if (((leftIndex + offset) >= size) || ((rightIndex + offset) >= size))
					{
						break;
					}
				}

				leftIndex += offset;
				rightIndex += offset;
			}

			leftIndex += 1;
			rightIndex += 1;
		}

		return Indices.size();
	}

	void DiffUtils::HexDump(const std::vector<U8>& Left, const std::vector<U8>& Right, const std::vector<std::pair<U64, U64>>& Indices, U32 ResultLimit, U64 Size, U32 Stride, U32 LeadStrideMultiplier)
	{
		U32 resultCount = 0;
		U32 offset = Stride * LeadStrideMultiplier;

		Size += offset;

		for (const auto& [leftIndex, rightIndex] : Indices)
		{
			LOG("  0x%08IX                                                                        0x%08IX\n", leftIndex, rightIndex);
			LOG("---------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

			for (U64 i = 0; i < Size; i += Stride)
			{
				if (i == offset)
				{
					LOG("> 0x%08IX | ", leftIndex + i - offset);
				}
				else
				{
					LOG("  0x%08IX | ", leftIndex + i - offset);
				}

				for (U64 j = 0; j < Stride; j++)
				{
					U64 index = leftIndex + i + j - offset;

					if (index >= 0 && index < Left.size())
					{
						LOG("%02X ", Left[index]);
					}
					else
					{
						LOG("%02X ", 0);
					}
				}

				LOG("| ");

				for (U64 j = 0; j < Stride; j++)
				{
					U64 index = leftIndex + i + j - offset;

					if (index >= 0 && index < Left.size())
					{
						if (Left[index] >= 32 && Left[index] < 127)
						{
							LOG("%c", Left[index]);
						}
						else
						{
							LOG(".");
						}
					}
					else
					{
						LOG(".");
					}
				}

				LOG(" | ");

				LOG("0x%08IX | ", rightIndex + i - offset);

				for (U64 j = 0; j < Stride; j++)
				{
					U64 index = rightIndex + i + j - offset;

					if (index >= 0 && index < Right.size())
					{
						LOG("%02X ", Right[index]);
					}
					else
					{
						LOG("%02X ", 0);
					}
				}

				LOG("| ");

				for (U64 j = 0; j < Stride; j++)
				{
					U64 index = rightIndex + i + j - offset;

					if (index >= 0 && index < Right.size())
					{
						if (Right[index] >= 32 && Right[index] < 127)
						{
							LOG("%c", Right[index]);
						}
						else
						{
							LOG(".");
						}
					}
					else
					{
						LOG(".");
					}
				}

				LOG("\n");
			}

			LOG("\n");

			resultCount += 1;

			if (resultCount > ResultLimit)
			{
				break;
			}
		}
	}
}
