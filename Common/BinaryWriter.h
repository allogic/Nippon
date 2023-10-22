#pragma once

#include <vector>
#include <string>
#include <cstring>

#include <Common/Types.h>
#include <Common/Macros.h>

namespace ark
{
	class BinaryWriter
	{
	public:

		BinaryWriter(U8* Bytes, U64 Size);

	public:

		inline const auto& GetBytes() const { return mBytes; }
		inline const auto& GetSize() const { return mSize; }
		inline const auto& GetPosition() const { return mPosition; }

	public:

		inline void SeekRel(I64 Value) { mPosition += Value; }
		inline void SeekAbs(I64 Value) { mPosition = Value; }

		inline void AlignUp(U64 Alignment) { mPosition = ALIGN_UP(mPosition, Alignment); }
		inline void AlignDown(U64 Alignment) { mPosition = ALIGN_DOWN(mPosition, Alignment); }

		inline void ModUp(U64 Modulus) { mPosition += (mPosition % Modulus); }
		inline void ModDown(U64 Modulus) { mPosition -= (mPosition % Modulus); }

	public:

		template<typename T>
		void Write(T Value);

		template<typename T>
		void Write(const std::vector<T>& Values);

		void Repeat(U8 Char, U64 Size);
		void String(const std::string& Value, U64 Size);
		void Bytes(U8* Bytes, U64 Size);

		void Zero(U64 Size);
		void ZeroNoInc(U64 Size);

	private:

		U8* mBytes;
		U64 mSize;

		U64 mPosition = 0;
	};
}

namespace ark
{
	template<typename T>
	void BinaryWriter::Write(T Value)
	{
		std::memcpy(mBytes + mPosition, &Value, sizeof(T));

		mPosition += sizeof(T);
	}

	template<typename T>
	void BinaryWriter::Write(const std::vector<T>& Values)
	{
		std::memcpy(mBytes + mPosition, Values.data(), Values.size() * sizeof(T));

		mPosition += Values.size() * sizeof(T);
	}
}
