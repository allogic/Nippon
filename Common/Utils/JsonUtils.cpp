#include <Common/Utils/JsonUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	std::set<std::string> JsonUtils::ToStringSet(const rj::Value& Value)
	{
		std::set<std::string> result;

		for (auto it = Value.Begin(); it != Value.End(); it++)
		{
			result.emplace(it->GetString());
		}

		return result;
	}
}