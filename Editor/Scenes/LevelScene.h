#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>

#include <Common/Types.h>

#include <Common/Generated/SceneInfos.h>

#include <Editor/Scene.h>

#include <Editor/Structs/ScrStructs.h>
#include <Editor/Structs/MdStructs.h>
#include <Editor/Structs/ObjStructs.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Archive;
	class Actor;
	class Texture2D;

	class LevelScene : public Scene
	{
	public:

		LevelScene(const SceneInfo& Info);
		virtual ~LevelScene();

	public:

		inline const auto& GetLevelGeometryActor() const { return mLevelGeometryActor; }
		inline const auto& GetEntityGeometryActor() const { return mEntityGeometryActor; }
		inline const auto& GetScrGroups() const { return mScrGroups; }
		inline const auto& GetScrTextures() const { return mScrTextures; }
		inline const auto& GetTscEntities() const { return mTscEntities; }
		inline const auto& GetTreEntities() const { return mTreEntities; }
		inline const auto& GetTatEntities() const { return mTatEntities; }

	public:

		virtual void Load() override;
		virtual void Save() override;

	private:

		void LoadArchives();
		void LoadLevel();
		void LoadEntities();

	private:

		void AddStaticGeometry();

	private:

		SceneInfo* ObjectToSceneInfo(const ObjEntry& Object);
		void PrintSummary();

	private:

		Actor* mLevelGeometryActor = nullptr;
		Actor* mEntityGeometryActor = nullptr;

	private:

		Archive* mDatArchive = nullptr;
		Archive* mBinArchive = nullptr;

	private:

		Archive* mTscArchive = nullptr;
		Archive* mTreArchive = nullptr;
		Archive* mTatArchive = nullptr;

		std::vector<Archive*> mScrArchives = {};
		std::vector<Archive*> mDdsArchives = {};

		std::vector<ScrGroup> mScrGroups = {};
		std::vector<Texture2D*> mScrTextures = {};

	private:

		struct EntityData
		{
			bool Loaded = false;

			SceneInfo* SceneInfo = nullptr;

			std::vector<Archive*> MdArchives = {};
			std::vector<Archive*> DdsArchives = {};

			std::vector<MdGroup> MdGroups = {};
			std::vector<Texture2D*> MdTextures = {};
		};

		std::map<std::string, Archive*> mTscArchiveCache = {};
		std::map<std::string, Archive*> mTreArchiveCache = {};
		std::map<std::string, Archive*> mTatArchiveCache = {};

		std::map<Archive*, EntityData> mTscEntityDataCache = {};
		std::map<Archive*, EntityData> mTreEntityDataCache = {};
		std::map<Archive*, EntityData> mTatEntityDataCache = {};

		struct Entity
		{
			U32 Index;
			ObjEntry Object;
			Archive* Archive;
		};

		std::vector<Entity> mTscEntities = {};
		std::vector<Entity> mTreEntities = {};
		std::vector<Entity> mTatEntities = {};
	};
}
