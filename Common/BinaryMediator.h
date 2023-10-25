#pragma once

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <Common/Types.h>
#include <Common/Macros.h>

namespace ark
{
	class BinaryMediator
	{
	public:

		BinaryMediator(U8* Bytes, U64 Size);
		BinaryMediator(const U8* Bytes, U64 Size);

	public:

		inline const auto& GetPosition() const { return mPosition; }

	public:

		inline void SeekRel(I64 Value) { mPosition += Value; }
		inline void SeekAbs(U64 Value) { mPosition = Value; }

		inline void AlignUp(U64 Alignment) { mPosition = ALIGN_UP(mPosition, Alignment); }
		inline void AlignDown(U64 Alignment) { mPosition = ALIGN_DOWN(mPosition, Alignment); }

		inline void ModUp(U64 Modulus) { mPosition -= mPosition % Modulus; }
		inline void ModDown(U64 Modulus) { mPosition += mPosition % Modulus; }

	public:

		template<typename T>
		T Read();

		template<typename T>
		std::vector<T> Read(U64 Count);

		template<typename T>
		void Read(std::vector<T>& Result, U64 Count);

		std::vector<U8> ReadByteRange(U64 Size);
		std::string ReadStringRange(U64 Size);

	public:

		template<typename T>
		void Write(T Value);

		template<typename T>
		void Write(const std::vector<T>& Values);

		void WriteByteRange(const U8* Bytes, U64 Size);
		void WriteStringRange(const I8* Bytes, U64 Size);

	private:

		U8* mBytes;
		U64 mSize;

		U64 mPosition = 0;
	};
}

namespace ark
{
	template<typename T>
	T BinaryMediator::Read()
	{
		T result = *((T*)(mBytes + mPosition));

		mPosition += sizeof(T);

		return result;
	}

	template<typename T>
	std::vector<T> BinaryMediator::Read(U64 Count)
	{
		std::vector<T> result = {};

		result.resize(Count);

		std::memcpy(result.data(), mBytes + mPosition, result.size() * sizeof(T));

		mPosition += result.size() * sizeof(T);

		return result;
	}

	template<typename T>
	void BinaryMediator::Read(std::vector<T>& Result, U64 Count)
	{
		Result.resize(Count);

		std::memcpy(Result.data(), mBytes + mPosition, Result.size() * sizeof(T));

		mPosition += Result.size() * sizeof(T);
	}

	template<typename T>
	void BinaryMediator::Write(T Value)
	{
		std::memcpy(mBytes + mPosition, &Value, sizeof(T));

		mPosition += sizeof(T);
	}

	template<typename T>
	void BinaryMediator::Write(const std::vector<T>& Values)
	{
		std::memcpy(mBytes + mPosition, Values.data(), Values.size() * sizeof(T));

		mPosition += Values.size() * sizeof(T);
	}
}
