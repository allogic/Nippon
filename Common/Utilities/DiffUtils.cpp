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

				// TODO: Implement clever window compare search!

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
		U32 fileOffset = Stride * LeadStrideMultiplier;

		Size += fileOffset;

		LOG("\n");

		for (const auto& [leftIndex, rightIndex] : Indices)
		{
			LOG("  0x%08IX", leftIndex);
			for (U32 i = 0; i < (3 + (Stride * 3) + 2 + Stride + 3); i++)
			{
				LOG(" ");
			}
			LOG("0x%08IX\n", rightIndex);
			for (U32 i = 0; i < ((2 + 10 + 3 + (Stride * 3) + 2 + Stride + 2) + (1 + 10 + 3 + (Stride * 3) + 2 + Stride + 2)); i++)
			{
				LOG("-");
			}
			LOG("\n");

			for (I64 i = 0; i < (I64)Size; i += Stride)
			{
				{
					I64 index = ALIGN_DOWN(leftIndex, (U64)Stride) + i - fileOffset;

					if (i == fileOffset)
					{
						LOG("> 0x%08IX | ", (index < 0) ? 0 : index);
					}
					else
					{
						LOG("  0x%08IX | ", (index < 0) ? 0 : index);
					}
				}

				for (I64 j = 0; j < Stride; j++)
				{
					I64 index = ALIGN_DOWN(leftIndex, (U64)Stride) + i + j - fileOffset;

					if (index >= 0 && index < (I64)Left.size())
					{
						LOG("%02X ", Left[index]);
					}
					else
					{
						LOG("%02X ", 0);
					}
				}

				LOG("| ");

				for (I64 j = 0; j < Stride; j++)
				{
					I64 index = ALIGN_DOWN(leftIndex, (U64)Stride) + i + j - fileOffset;

					if (index >= 0 && index < (I64)Left.size())
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

				{
					I64 index = ALIGN_DOWN(rightIndex, (U64)Stride) + i - fileOffset;

					LOG("0x%08IX | ", (index < 0) ? 0 : index);
				}

				for (I64 j = 0; j < Stride; j++)
				{
					I64 index = ALIGN_DOWN(rightIndex, 16) + i + j - fileOffset;

					if (index >= 0 && index < (I64)Right.size())
					{
						LOG("%02X ", Right[index]);
					}
					else
					{
						LOG("%02X ", 0);
					}
				}

				LOG("| ");

				for (I64 j = 0; j < Stride; j++)
				{
					I64 index = ALIGN_DOWN(rightIndex, 16) + i + j - fileOffset;

					if (index >= 0 && index < (I64)Right.size())
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

			if (resultCount >= ResultLimit)
			{
				break;
			}
		}
	}
}
