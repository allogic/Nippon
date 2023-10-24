#include <Common/CommandLine.h>

namespace ark
{
	struct ArgumentBySubString
	{
		std::string SubString;

		bool operator () (const std::string& Argument) const
		{
			return Argument.compare(0, SubString.size(), SubString, 0, SubString.size()) == 0;
		}
	};

	static std::vector<std::string> sArguments = {};

	void CommandLine::Init(U32 Argc, I8** Argv)
	{
		sArguments = { Argv + 1, Argv + Argc };
	}

	bool CommandLine::HasArgument(const std::string Argument)
	{
		const auto findIt = std::find_if(sArguments.begin(), sArguments.end(), ArgumentBySubString{ Argument });

		if (findIt != sArguments.end())
		{
			return true;
		}

		return false;
	}

	bool CommandLine::HasFirstArgument(const std::string Argument)
	{
		if (sArguments.size() > 0)
		{
			return sArguments[0] == Argument;
		}

		return false;
	}

	bool CommandLine::HasArgumentWithValue(const std::string Argument, std::string& Value, const std::string& ErrorMessage)
	{
		const auto findIt = std::find_if(sArguments.begin(), sArguments.end(), ArgumentBySubString{ Argument });

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
			LOG("\n");
			LOG("%s\n", ErrorMessage.c_str());
			LOG("\n");
		}

		return false;
	}
}
