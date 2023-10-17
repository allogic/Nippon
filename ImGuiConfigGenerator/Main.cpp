#include <sstream>

#include <Common/Types.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

#include <ImGuiConfigGenerator/Generated/SceneInfos.h>

using namespace ark;

void WriteConfig(const std::string& FileName) {
	std::ostringstream oss;

	for (const auto& groupInfo : SceneInfos::GetLevelGroups())
	{
		for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
		{
			oss << "[Window][" << sceneInfo.WindowName << "]\n";
			oss << "Pos=308,28\n";
			oss << "Size=1044,1052\n";
			oss << "Collapsed=0\n";
			oss << "DockId=0x00000002,0\n";
			oss << "\n";
		}
	}

	for (const auto& groupInfo : SceneInfos::GetEntityGroups())
	{
		for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
		{
			oss << "[Window][" << sceneInfo.WindowName << "]\n";
			oss << "Pos=308,28\n";
			oss << "Size=1044,1052\n";
			oss << "Collapsed=0\n";
			oss << "DockId=0x00000002,0\n";
			oss << "\n";
		}
	}

	FsUtils::WriteText(FileName + ".ini", oss.str().c_str());
}

I32 main(I32 Argc, char** Argv)
{
	WriteConfig("imgui");

	return 0;
}
