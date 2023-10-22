#pragma once

#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <map>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class BlowFish;
	class DebugRenderer;
	class DefaultRenderer;

	extern fs::path gDataDir;
	
	extern BlowFish* gBlowFish;

	extern R32 gTimeDelta;

	extern DebugRenderer* gDebugRenderer;
	extern DefaultRenderer* gDefaultRenderer;
}
