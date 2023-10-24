#include <Common/BinaryWriter.h>

namespace ark
{
	void BinaryWriter::FillRange(U8 Byte, U64 Size)
	{
		for (U64 i = 0; i < Size; i++)
		{
			write((I8*)&Byte, sizeof(U8));
		}
	}

	void BinaryWriter::ByteRange(const U8* Bytes, U64 Size)
	{
		for (U64 i = 0; i < Size; i++)
		{
			write((I8*)(Bytes + i), sizeof(U8));
		}
	}

	void BinaryWriter::StringRange(const I8* Bytes, U64 Size)
	{
		for (U64 i = 0; i < Size; i++)
		{
			write(Bytes + i, sizeof(I8));
		}
	}

	void BinaryWriter::CopyDataInto(U8* Bytes)
	{
		const std::string& string = str();

		std::memcpy(Bytes, string.data(), string.size());
	}
}
