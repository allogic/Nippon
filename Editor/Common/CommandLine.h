#pragma once

#include <Types.h>
#include <Standard.h>

namespace Nippon
{
	class CommandLine
	{
	public:

		static void Init(U32 Argc, char** Argv);

		static bool HasArgument(std::string const& Argument);
		static bool HasFirstArgument(std::string const& Argument);
		static bool HasArgumentWithValue(std::string const& Argument, std::string& Value, std::string const& ErrorMessage = "");
	};
}