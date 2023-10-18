#include <Common/BinaryReader.h>

namespace ark
{
	BinaryReader::BinaryReader()
		: mStart{ nullptr }
		, mEnd{ nullptr }
		, mPosition{ 0 }
	{

	}

	BinaryReader::BinaryReader(const U8* Start, const U8* End)
		: mStart{ Start }
		, mEnd{ End }
		, mPosition{ 0 }
	{

	}

	std::vector<U8> BinaryReader::Bytes(U64 Count)
	{
		std::vector<U8> bytes = {};

		bytes.resize(Count);

		if (Count > 0)
		{
			std::memcpy(&bytes[0], mStart + mPosition, Count);
		}

		mPosition += Count;

		return bytes;
	}

	std::vector<U8> BinaryReader::BytesFrom(U64 Count, U64 Offset) const
	{
		std::vector<U8> bytes = {};

		bytes.resize(Count);

		if (Count > 0)
		{
			std::memcpy(&bytes[0], mStart + Offset, Count);
		}

		return bytes;
	}

	std::string BinaryReader::String(U64 Count)
	{
		std::string string = {};

		string.resize(Count);

		if (Count > 0)
		{
			std::memcpy(&string[0], mStart + mPosition, Count);
		}

		mPosition += Count;

		return string;
	}

	std::string BinaryReader::StringFrom(U64 Count, U64 Offset) const
	{
		std::string string = {};

		string.resize(Count);

		if (Count > 0)
		{
			std::memcpy(&string[0], mStart + Offset, Count);
		}

		return string;
	}
}
