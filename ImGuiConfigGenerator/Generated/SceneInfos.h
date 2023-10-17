#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace ark
{
	typedef enum
	{
		eSceneTypeNone = 0,
		eSceneTypeLevel = 1,
		eSceneTypeEntity = 2,
	} SceneType;

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
		std::string DatArchiveFileName;
		std::string BinArchiveFileName;
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
		static GroupInfo* GetLevelGroupByKey(const std::string& GroupKey);
		static SceneInfo* GetLevelByKey(const std::string& GroupKey, const std::string& SceneKey);

		static const std::vector<GroupInfo>& GetEntityGroups();
		static const std::vector<SceneInfo>& GetEntitiesByGroup(const GroupInfo& Group);
		static GroupInfo* GetEntityGroupByKey(const std::string& EntityKey);
		static SceneInfo* GetEntityByKey(const std::string& GroupKey, const std::string& SceneKey);

		static bool LevelGroupContainsSceneKey(const std::string& GroupKey, const std::string& SceneKey);
		static bool EntityGroupContainsSceneKey(const std::string& GroupKey, const std::string& SceneKey);
	};
}
