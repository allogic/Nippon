#pragma once

#include <vector>
#include <string>
#include <cstring>

#include <Common/Types.h>

namespace ark
{
	class BinaryReader
	{
	public:

		BinaryReader();
		BinaryReader(const U8* Start, const U8* End);

	public:

		inline const auto& GetPosition() const { return mPosition; }

	public:

		inline void SeekRel(I64 Value) { mPosition += Value; }
		inline void SeekAbs(I64 Value) { mPosition = Value; }

	public:

		template<typename T>
		T Read();

		template<typename T>
		std::vector<T> Read(U64 Count);

		template<typename T>
		void Read(std::vector<T>& Values, U64 Count);

		std::vector<U8> Bytes(U64 Count);
		std::vector<U8> BytesFrom(U64 Count, U64 Offset) const;

		std::string String(U64 Count);
		std::string StringFrom(U64 Count, U64 Offset) const;

	private:

		const U8* mStart;
		const U8* mEnd;

		U64 mPosition;
	};
}

namespace ark
{
	template<typename T>
	T BinaryReader::Read()
	{
		T value = {};

		std::memcpy((U8*)&value, mStart + mPosition, sizeof(T));

		mPosition += sizeof(T);

		return value;
	}

	template<typename T>
	std::vector<T> BinaryReader::Read(U64 Count)
	{
		std::vector<T> values = {};

		values.resize(Count);

		if (Count > 0)
		{
			std::memcpy((U8*)&values[0], mStart + mPosition, Count * sizeof(T));
		}

		mPosition += Count * sizeof(T);

		return values;
	}

	template<typename T>
	void BinaryReader::Read(std::vector<T>& Values, U64 Count)
	{
		Values.resize(Count);

		if (Count > 0)
		{
			std::memcpy((U8*)&Values[0], mStart + mPosition, Count * sizeof(T));
		}

		mPosition += Count * sizeof(T);
	}
}
