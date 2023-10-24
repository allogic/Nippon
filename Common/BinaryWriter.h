#pragma once

#include <cstdlib>
#include <vector>
#include <sstream>

#include <Common/Types.h>
#include <Common/Macros.h>

namespace ark
{
	class BinaryWriter : private std::ostringstream
	{
	public:

		inline const auto GetBytes() const { return (U8*)rdbuf()->view().data(); }
		inline const auto GetSize() const { return rdbuf()->view().size(); }

	public:

		inline void SeekRel(I64 Value) { seekp(Value, std::ios_base::cur); }
		inline void SeekAbs(U64 Value) { seekp(Value, std::ios_base::beg); }

		inline void AlignUp(U64 Alignment) { seekp(ALIGN_UP((U64)tellp(), Alignment), std::ios_base::beg); }
		inline void AlignDown(U64 Alignment) { seekp(ALIGN_DOWN((U64)tellp(), Alignment), std::ios_base::beg); }

	public:

		template<typename T>
		void Write(T Value);

		template<typename T>
		void Write(const std::vector<T>& Values);

		void FillRange(U8 Byte, U64 Size);
		void ByteRange(const U8* Bytes, U64 Size);
		void StringRange(const I8* Bytes, U64 Size);

	public:

		void CopyDataInto(U8* Bytes);
	};
}

namespace ark
{
	template<typename T>
	void BinaryWriter::Write(T Value)
	{
		write((I8*)&Value, sizeof(T));
	}

	template<typename T>
	void BinaryWriter::Write(const std::vector<T>& Values)
	{
		write((I8*)Values.data(), Values.size() * sizeof(T));
	}
}
