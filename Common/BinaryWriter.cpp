#include <Common/BinaryWriter.h>

namespace ark
{
	BinaryWriter::BinaryWriter(U8* Bytes, U64 Size)
		: mBytes{ Bytes }
		, mSize{ Size }
	{

	}

	void BinaryWriter::Repeat(U8 Char, U64 Size)
	{
		for (U64 i = 0; i < Size; i++)
		{
			*(mBytes + mPosition + i) = Char;
		}

		mPosition += Size;
	}

	void BinaryWriter::String(const std::string& Value, U64 Size)
	{
		std::memset(mBytes + mPosition, 0, Size);
		std::memcpy(mBytes + mPosition, Value.data(), Value.size());

		mPosition += Size;
	}

	void BinaryWriter::Bytes(U8* Bytes, U64 Size)
	{
		std::memcpy(mBytes + mPosition, Bytes, Size);

		mPosition += Size;
	}

	void BinaryWriter::Zero(U64 Size)
	{
		std::memset(mBytes + mPosition, 0, Size);

		mPosition += Size;
	}

	void BinaryWriter::ZeroNoInc(U64 Size)
	{
		std::memset(mBytes + mPosition, 0, Size);
	}
}
