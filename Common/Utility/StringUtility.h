#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

namespace Nippon
{
	class StringUtility
	{
	public:

		static std::string CutFront(std::string const& String, U64 Size);
		static std::string CutBack(std::string const& String, U64 Size);

		static std::string RemoveChars(std::string const& String, I8 Char);

		static std::string ToLower(std::string const& String);
		static std::string ToUpper(std::string const& String);

		static std::string SelectExpr(std::string const& String, std::string const& Expression);

		static std::string ByteToHexStr(U8 Byte);
		static U8 HexStrToByte(std::string const& String);
	};
}