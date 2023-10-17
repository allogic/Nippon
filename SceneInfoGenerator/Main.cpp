#include <sstream>

#include <Common/Types.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

#include <SceneInfoGenerator/Generated/SceneInfos.h>

using namespace ark;

void WriteHeader(const std::string& FileName) {
	std::ostringstream oss;

	oss << "#pragma once\n";
	oss << "\n";
	oss << "#include <string>\n";
	oss << "#include <vector>\n";
	oss << "#include <map>\n";
	oss << "#include <algorithm>\n";
	oss << "\n";
	oss << "namespace ark\n";
	oss << "{\n";
	oss << "\ttypedef enum\n";
	oss << "\t{\n";
	oss << "\t\teSceneTypeNone = 0,\n";
	oss << "\t\teSceneTypeLevel = 1,\n";
	oss << "\t\teSceneTypeEntity = 2,\n";
	oss << "\t} SceneType;\n";
	oss << "\n";
	oss << "\ttypedef struct\n";
	oss << "\t{\n";
	oss << "\t\tstd::string GroupKey;\n";
	oss << "\t\tstd::string Name;\n";
	oss << "\t\tstd::string MenuName;\n";
	oss << "\t} GroupInfo;\n";
	oss << "\n";
	oss << "\ttypedef struct\n";
	oss << "\t{\n";
	oss << "\t\tSceneType Type;\n";
	oss << "\t\tstd::string GroupKey;\n";
	oss << "\t\tstd::string SceneKey;\n";
	oss << "\t\tstd::string ArchiveFileName;\n";
	oss << "\t\tstd::string ThumbnailFileName;\n";
	oss << "\t\tstd::string Name;\n";
	oss << "\t\tstd::string MenuName;\n";
	oss << "\t\tstd::string WindowName;\n";
	oss << "\t} SceneInfo;\n";
	oss << "\n";
	oss << "\tclass " << FileName << "\n";
	oss << "\t{\n";
	oss << "\tpublic:\n";
	oss << "\n";
	oss << "\t\tstatic const std::vector<GroupInfo>& GetLevelGroups();\n";
	oss << "\t\tstatic const std::vector<SceneInfo>& GetLevelsByGroup(const GroupInfo& Group);\n";
	oss << "\t\tstatic GroupInfo* GetLevelGroupByKey(const std::string& GroupKey);\n";
	oss << "\t\tstatic SceneInfo* GetLevelByKey(const std::string& GroupKey, const std::string& SceneKey);\n";
	oss << "\n";
	oss << "\t\tstatic const std::vector<GroupInfo>& GetEntityGroups();\n";
	oss << "\t\tstatic const std::vector<SceneInfo>& GetEntitiesByGroup(const GroupInfo& Group);\n";
	oss << "\t\tstatic GroupInfo* GetEntityGroupByKey(const std::string& EntityKey);\n";
	oss << "\t\tstatic SceneInfo* GetEntityByKey(const std::string& GroupKey, const std::string& SceneKey);\n";
	oss << "\n";
	oss << "\t\tstatic bool LevelGroupContainsSceneKey(const std::string& GroupKey, const std::string& SceneKey);\n";
	oss << "\t\tstatic bool EntityGroupContainsSceneKey(const std::string& GroupKey, const std::string& SceneKey);\n";
	oss << "\t};\n";
	oss << "}\n";

	FsUtils::WriteText(FileName + ".h", oss.str().c_str());
}

void WriteSource(const std::string& FileName) {
	std::ostringstream oss;

	oss << "#include <Editor/Generated/" << FileName << ".h>\n";
	oss << "\n";
	oss << "namespace ark\n";
	oss << "{\n";

	oss << "\tstruct SceneInfoByKey\n";
	oss << "\t{\n";
	oss << "\t\tstd::string SceneKey;\n";
	oss << "\n";
	oss << "\t\tbool operator () (const SceneInfo& Info) const\n";
	oss << "\t\t{\n";
	oss << "\t\t\treturn SceneKey == Info.SceneKey;\n";
	oss << "\t\t}\n";
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::vector<GroupInfo> sLevelGroups =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetLevelGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			std::string groupName = (groupInfo[i].Name == "") ? "Unknown" : groupInfo[i].Name;
			oss << "\t\tGroupInfo{ \"" << groupInfo[i].GroupKey << "\", \"" << groupName << "\", \"" << groupInfo[i].GroupKey << " - " << groupName << "\" },\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::map<std::string, std::vector<SceneInfo>> sLevelsByGroup =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetLevelGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			oss << "\t\t{ \"" << groupInfo[i].GroupKey << "\",\n";
			oss << "\t\t\t{\n";
			const auto& sceneInfo = SceneInfos::GetLevelsByGroup(groupInfo[i]);
			for (U32 j = 0; j < sceneInfo.size(); ++j)
			{
				std::string levelName = (sceneInfo[j].Name == "") ? "Unknown" : sceneInfo[j].Name;
				std::string archiveFileName = "r" + StringUtils::CutFront(sceneInfo[j].GroupKey, 2) + sceneInfo[j].SceneKey + ".dat";
				std::string thumbnailFileName = "r" + StringUtils::CutFront(sceneInfo[j].GroupKey, 2) + sceneInfo[j].SceneKey + ".png";
				oss << "\t\t\t\tSceneInfo{ eSceneTypeLevel, \"" << sceneInfo[j].GroupKey << "\", \"" << sceneInfo[j].SceneKey << "\", \"" << archiveFileName << "\", \"" << thumbnailFileName << "\", \"" << levelName << "\", \"" << sceneInfo[j].SceneKey << " - " << levelName << "\", \"/" << sceneInfo[j].GroupKey << "/" << sceneInfo[j].SceneKey << " - " << levelName << "\" },\n";
			}
			oss << "\t\t\t},\n";
			oss << "\t\t},\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::map<std::string, GroupInfo*> sLevelGroupsByKey =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetLevelGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			oss << "\t\t{ \"" << groupInfo[i].GroupKey << "\", &sLevelGroups[" << i << "] },\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::map<std::string, std::map<std::string, SceneInfo*>> sLevelsByKey =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetLevelGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			oss << "\t\t{ \"" << groupInfo[i].GroupKey << "\",\n";
			oss << "\t\t\t{\n";
			const auto& sceneInfo = SceneInfos::GetLevelsByGroup(groupInfo[i]);
			for (U32 j = 0; j < sceneInfo.size(); ++j)
			{
				oss << "\t\t\t\t{ \"" << sceneInfo[j].SceneKey << "\", &sLevelsByGroup[\"" << groupInfo[i].GroupKey << "\"][" << j << "] },\n";
			}
			oss << "\t\t\t},\n";
			oss << "\t\t},\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::vector<GroupInfo> sEntityGroups =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetEntityGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			std::string groupName = (groupInfo[i].Name == "") ? "Unknown" : groupInfo[i].Name;
			oss << "\t\tGroupInfo{ \"" << groupInfo[i].GroupKey << "\", \"" << groupName << "\", \"" << groupInfo[i].GroupKey << " - " << groupName << "\" },\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::map<std::string, std::vector<SceneInfo>> sEntitiesByGroup =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetEntityGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			oss << "\t\t{ \"" << groupInfo[i].GroupKey << "\",\n";
			oss << "\t\t\t{\n";
			const auto& sceneInfo = SceneInfos::GetEntitiesByGroup(groupInfo[i]);
			for (U32 j = 0; j < sceneInfo.size(); j++)
			{
				std::string entityName = (sceneInfo[j].Name == "") ? "Unknown" : sceneInfo[j].Name;
				std::string archiveFileName = sceneInfo[j].GroupKey + sceneInfo[j].SceneKey + ".dat";
				std::string thumbnailFileName = sceneInfo[j].GroupKey + sceneInfo[j].SceneKey + ".png";
				oss << "\t\t\t\tSceneInfo{ eSceneTypeEntity, \"" << sceneInfo[j].GroupKey << "\", \"" << sceneInfo[j].SceneKey << "\", \"" << archiveFileName << "\", \"" << thumbnailFileName << "\", \"" << entityName << "\", \"" << sceneInfo[j].SceneKey << " - " << entityName << "\", \"/" << sceneInfo[j].GroupKey << "/" << sceneInfo[j].SceneKey << " - " << entityName << "\" },\n";
			}
			oss << "\t\t\t},\n";
			oss << "\t\t},\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::map<std::string, GroupInfo*> sEntityGroupsByKey =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetEntityGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			oss << "\t\t{ \"" << groupInfo[i].GroupKey << "\", &sEntityGroups[" << i << "] },\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tstatic std::map<std::string, std::map<std::string, SceneInfo*>> sEntitiesByKey =\n";
	oss << "\t{\n";
	{
		const auto& groupInfo = SceneInfos::GetEntityGroups();
		for (U32 i = 0; i < groupInfo.size(); i++)
		{
			oss << "\t\t{ \"" << groupInfo[i].GroupKey << "\",\n";
			oss << "\t\t\t{\n";
			const auto& sceneInfo = SceneInfos::GetEntitiesByGroup(groupInfo[i]);
			for (U32 j = 0; j < sceneInfo.size(); ++j)
			{
				oss << "\t\t\t\t{ \"" << sceneInfo[j].SceneKey << "\", &sEntitiesByGroup[\"" << groupInfo[i].GroupKey << "\"][" << j << "] },\n";
			}
			oss << "\t\t\t},\n";
			oss << "\t\t},\n";
		}
	}
	oss << "\t};\n";
	oss << "\n";

	oss << "\tconst std::vector<GroupInfo>& " << FileName << "::GetLevelGroups()\n";
	oss << "\t{\n";
	oss << "\t\treturn sLevelGroups;\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tconst std::vector<SceneInfo>& " << FileName << "::GetLevelsByGroup(const GroupInfo& Group)\n";
	oss << "\t{\n";
	oss << "\t\treturn sLevelsByGroup[Group.GroupKey];\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tGroupInfo* " << FileName << "::GetLevelGroupByKey(const std::string& GroupKey)\n";
	oss << "\t{\n";
	oss << "\t\treturn sLevelGroupsByKey[GroupKey];\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tSceneInfo* " << FileName << "::GetLevelByKey(const std::string& GroupKey, const std::string& SceneKey)\n";
	oss << "\t{\n";
	oss << "\t\treturn sLevelsByKey[GroupKey][SceneKey];\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tconst std::vector<GroupInfo>& " << FileName << "::GetEntityGroups()\n";
	oss << "\t{\n";
	oss << "\t\treturn sEntityGroups;\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tconst std::vector<SceneInfo>& " << FileName << "::GetEntitiesByGroup(const GroupInfo& Group)\n";
	oss << "\t{\n";
	oss << "\t\treturn sEntitiesByGroup[Group.GroupKey];\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tGroupInfo* " << FileName << "::GetEntityGroupByKey(const std::string& GroupKey)\n";
	oss << "\t{\n";
	oss << "\t\treturn sEntityGroupsByKey[GroupKey];\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tSceneInfo* " << FileName << "::GetEntityByKey(const std::string& GroupKey, const std::string& SceneKey)\n";
	oss << "\t{\n";
	oss << "\t\treturn sEntitiesByKey[GroupKey][SceneKey];\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tbool " << FileName << "::LevelGroupContainsSceneKey(const std::string& GroupKey, const std::string& SceneKey)\n";
	oss << "\t{\n";
	oss << "\t\treturn std::find_if(sLevelsByGroup[GroupKey].begin(), sLevelsByGroup[GroupKey].end(), SceneInfoByKey{ SceneKey }) != sLevelsByGroup[GroupKey].end();\n";
	oss << "\t}\n";
	oss << "\n";

	oss << "\tbool " << FileName << "::EntityGroupContainsSceneKey(const std::string& GroupKey, const std::string& SceneKey)\n";
	oss << "\t{\n";
	oss << "\t\treturn std::find_if(sEntitiesByGroup[GroupKey].begin(), sEntitiesByGroup[GroupKey].end(), SceneInfoByKey{ SceneKey }) != sEntitiesByGroup[GroupKey].end();\n";
	oss << "\t}\n";

	oss << "}\n";

	FsUtils::WriteText(FileName + ".cpp", oss.str().c_str());
}

I32 main(I32 Argc, char** Argv)
{
	WriteHeader("SceneInfos");
	WriteSource("SceneInfos");

	return 0;
}
