#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

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