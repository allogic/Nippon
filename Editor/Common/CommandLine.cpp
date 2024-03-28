#include <Common/CommandLine.h>

namespace Nippon
{
	struct ArgumentBySubString
	{
		std::string SubString;

		bool operator () (std::string const& Argument) const
		{
			return Argument.compare(0, SubString.size(), SubString, 0, SubString.size()) == 0;
		}
	};

	static std::vector<std::string> sArguments = {};

	void CommandLine::Init(U32 Argc, char** Argv)
	{
		sArguments = { Argv + 1, Argv + Argc };
	}

	bool CommandLine::HasArgument(std::string const& Argument)
	{
		auto const findIt = std::find_if(sArguments.begin(), sArguments.end(), ArgumentBySubString{ Argument });

		if (findIt != sArguments.end())
		{
			return true;
		}

		return false;
	}

	bool CommandLine::HasFirstArgument(std::string const& Argument)
	{
		if (sArguments.size() > 0)
		{
			return sArguments[0] == Argument;
		}

		return false;
	}

	bool CommandLine::HasArgumentWithValue(std::string const& Argument, std::string& Value, std::string const& ErrorMessage)
	{
		auto const findIt = std::find_if(sArguments.begin(), sArguments.end(), ArgumentBySubString{ Argument });

		if (findIt != sArguments.end())
		{
			std::string subject = *findIt;

			U64 equalSign = subject.find_first_of('=');

			if (equalSign != std::string::npos)
			{
				Value = subject.substr((equalSign + 1), subject.length() - equalSign);

				return !Value.empty();
			}
		}

		if (!ErrorMessage.empty())
		{
			std::printf("\n");
			std::printf("%s\n", ErrorMessage.data());
			std::printf("\n");
		}

		return false;
	}
}