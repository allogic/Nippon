#include <Common/BinaryMediator.h>

namespace ark
{
	BinaryMediator::BinaryMediator(U8* Bytes, U64 Size)
		: mBytes{ Bytes }
		, mSize{ Size }
	{

	}

	BinaryMediator::BinaryMediator(const U8* Bytes, U64 Size)
		: mBytes{ (U8*)Bytes }
		, mSize{ Size }
	{

	}

	std::vector<U8> BinaryMediator::ReadByteRange(U64 Size)
	{
		std::vector<U8> result = {};

		result.resize(Size);

		std::memcpy(result.data(), mBytes + mPosition, Size);

		mPosition += Size;

		return result;
	}

	std::string BinaryMediator::ReadStringRange(U64 Size)
	{
		std::string result = "";

		result.resize(Size);

		std::memcpy(result.data(), mBytes + mPosition, Size);

		mPosition += Size;

		return result;
	}

	void BinaryMediator::WriteByteRange(const U8* Bytes, U64 Size)
	{
		std::memcpy(mBytes + mPosition, Bytes, Size);

		mPosition += Size;
	}

	void BinaryMediator::WriteStringRange(const I8* Bytes, U64 Size)
	{
		std::memcpy(mBytes + mPosition, Bytes, Size);

		mPosition += Size;
	}
}
