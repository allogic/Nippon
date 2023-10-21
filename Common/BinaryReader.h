#pragma once

#include <vector>
#include <string>
#include <cstring>

#include <Common/Types.h>
#include <Common/Macros.h>

namespace ark
{
	class BinaryReader
	{
	public:

		BinaryReader(const U8* Bytes, U64 Size);

	public:

		inline const auto& GetBytes() const { return mBytes; }
		inline const auto& GetSize() const { return mSize; }
		inline const auto& GetPosition() const { return mPosition; }

	public:

		inline void SeekRel(I64 Value) { mPosition += Value; }
		inline void SeekAbs(I64 Value) { mPosition = Value; }

		inline void AlignUp(U64 Alignment) { mPosition = ALIGN_UP(mPosition, Alignment); }
		inline void AlignDown(U64 Alignment) { mPosition = ALIGN_DOWN(mPosition, Alignment); }

		inline void ModUp(U64 Modulus) { mPosition += mPosition % Modulus; }
		inline void ModDown(U64 Modulus) { mPosition -= mPosition % Modulus; }

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

		const U8* mBytes;
		U64 mSize;

		U64 mPosition = 0;
	};
}

namespace ark
{
	template<typename T>
	T BinaryReader::Read()
	{
		T value = {};

		std::memcpy(&value, mBytes + mPosition, sizeof(T));

		mPosition += sizeof(T);

		return value;
	}

	template<typename T>
	std::vector<T> BinaryReader::Read(U64 Count)
	{
		std::vector<T> values = {};

		values.resize(Count);

		std::memcpy(values.data(), mBytes + mPosition, Count * sizeof(T));

		mPosition += Count * sizeof(T);

		return values;
	}

	template<typename T>
	void BinaryReader::Read(std::vector<T>& Values, U64 Count)
	{
		Values.resize(Count);

		std::memcpy(Values.data(), mBytes + mPosition, Count * sizeof(T));

		mPosition += Count * sizeof(T);
	}
}
