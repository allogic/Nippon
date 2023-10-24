#include <Common/Utilities/StringUtils.h>

namespace ark
{
	std::string StringUtils::CutFront(const std::string& String, U64 Size)
	{
		return String.substr(Size);
	}

	std::string StringUtils::CutBack(const std::string& String, U64 Size)
	{
		return String.substr(0, String.size() - Size);
	}

	std::string StringUtils::RemoveChars(const std::string& String, I8 Char)
	{
		std::string result = String;

		result.erase(std::remove_if(result.begin(), result.end(), [=](I8 C) { return C == Char; }), result.end());

		return result;
	}

	std::string StringUtils::ToLower(const std::string& String)
	{
		std::string result = String;

		std::transform(result.begin(), result.end(), result.begin(), [](I8 C) { return std::tolower(C); });

		return result;
	}

	std::string StringUtils::ToUpper(const std::string& String)
	{
		std::string result = String;

		std::transform(result.begin(), result.end(), result.begin(), [](I8 C) { return std::toupper(C); });

		return result;
	}

	std::string StringUtils::SelectExpr(const std::string& String, const std::string& Expression)
	{
		std::stringstream stream;

		for (U32 i = 0; i < String.size(); i++)
		{
			if (i + Expression.size() <= String.size())
			{
				for (U32 j = 0; j < Expression.size(); j++)
				{
					if (Expression[j] == 'X')
					{
						stream << String[i + j];
					}
				}

				break;
			}
		}

		return stream.str();
	}

	std::string StringUtils::ByteToHexStr(U8 Byte)
	{
		I8 string[3] = {};

		std::sprintf(string, "%02x", Byte);

		return string;
	}

	U8 StringUtils::HexStrToByte(const std::string& String)
	{
		return (U8)std::strtol(String.c_str(), nullptr, 16);
	}
}
