#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>

#include <Common/Types.h>

namespace ark
{
	class StringUtils
	{
	public:

		static std::string CutFront(const std::string& String, U64 Size);
		static std::string CutBack(const std::string& String, U64 Size);

		static std::string RemoveChars(const std::string& String, I8 Char);

		static std::string ToLower(const std::string& String);
		static std::string ToUpper(const std::string& String);

		static std::string SelectExpr(const std::string& String, const std::string& Expression);

		static std::string ByteToHexStr(U8 Byte);
		static U8 HexStrToByte(const std::string& String);
	};
}
