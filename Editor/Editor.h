#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class DebugRenderer;
	class DefaultRenderer;

	extern fs::path gDataDir;
	
	extern std::string gBlowFishKey;

	extern R32 gTimeDelta;

	extern DebugRenderer* gDebugRenderer;
	extern DefaultRenderer* gDefaultRenderer;
}
