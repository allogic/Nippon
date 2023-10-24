#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>
#include <Common/Macros.h>

namespace ark
{
	class CommandLine
	{
	public:

		static void Init(U32 Argc, I8** Argv);

		static bool HasArgument(const std::string Argument);
		static bool HasFirstArgument(const std::string Argument);
		static bool HasArgumentWithValue(const std::string Argument, std::string& Value, const std::string& ErrorMessage = "");
	};
}
