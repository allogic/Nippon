#pragma once

#include <string>
#include <vector>
#include <map>

namespace ark
{
	enum SceneType
	{
		eSceneTypeNone = 0,
		eSceneTypeLevel = 1,
		eSceneTypeEntity = 2,
	};

	typedef struct
	{
		std::string GroupKey;
		std::string Name;
		std::string MenuName;
	} GroupInfo;

	typedef struct
	{
		SceneType Type;
		std::string GroupKey;
		std::string SceneKey;
		std::string ArchiveFileName;
		std::string ThumbnailFileName;
		std::string Name;
		std::string MenuName;
		std::string WindowName;
	} SceneInfo;

	class SceneInfos
	{
	public:

		static const std::vector<GroupInfo>& GetLevelGroups();
		static const std::vector<SceneInfo>& GetLevelsByGroup(const GroupInfo& Group);
		static const GroupInfo& GetLevelGroupByKey(const std::string& GroupKey);
		static const SceneInfo& GetLevelByKey(const std::string& GroupKey, const std::string& SceneKey);

		static const std::vector<GroupInfo>& GetEntityGroups();
		static const std::vector<SceneInfo>& GetEntitiesByGroup(const GroupInfo& Group);
		static const GroupInfo& GetEntityGroupByKey(const std::string& EntityKey);
		static const SceneInfo& GetEntityByKey(const std::string& GroupKey, const std::string& SceneKey);
	};
}
