#include <Utility/StringUtility.h>

namespace Nippon
{
	std::string StringUtility::CutFront(std::string const& String, U64 Size)
	{
		return String.substr(Size);
	}

	std::string StringUtility::CutBack(std::string const& String, U64 Size)
	{
		return String.substr(0, String.size() - Size);
	}

	std::string StringUtility::RemoveChars(std::string const& String, I8 Char)
	{
		std::string result = String;

		result.erase(std::remove_if(result.begin(), result.end(), [=](I8 C) { return C == Char; }), result.end());

		return result;
	}

	std::string StringUtility::ToLower(std::string const& String)
	{
		std::string result = String;

		std::transform(result.begin(), result.end(), result.begin(), [](I8 C) { return std::tolower(C); });

		return result;
	}

	std::string StringUtility::ToUpper(std::string const& String)
	{
		std::string result = String;

		std::transform(result.begin(), result.end(), result.begin(), [](I8 C) { return std::toupper(C); });

		return result;
	}

	std::string StringUtility::SelectExpr(std::string const& String, std::string const& Expression)
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

	std::string StringUtility::ByteToHexStr(U8 Byte)
	{
		I8 string[3] = {};

		std::sprintf(string, "%02x", Byte);

		return string;
	}

	U8 StringUtility::HexStrToByte(std::string const& String)
	{
		return (U8)std::strtol(String.data(), nullptr, 16);
	}
}