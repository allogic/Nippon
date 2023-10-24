#pragma once

#include <cstdio>
#include <string>
#include <random>

#include <Common/Types.h>

namespace ark
{
	class UUID
	{
	public:

		static std::string Generate();
	};
}
