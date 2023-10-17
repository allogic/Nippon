#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>

#include <Common/Types.h>

#include <Editor/Scene.h>

#include <Editor/Generated/SceneInfos.h>

#include <Editor/Structs/ScrStructs.h>
#include <Editor/Structs/MdStructs.h>
#include <Editor/Structs/ObjStructs.h>

namespace ark
{
	namespace fs = std::filesystem;

	class BlowFish;
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

		BlowFish* mCipher = nullptr;

		Archive* mDatArchive = nullptr;
		Archive* mBinArchive = nullptr;

	private:

		Archive* mTscNode = nullptr;
		Archive* mTreNode = nullptr;
		Archive* mTatNode = nullptr;

		std::vector<Archive*> mScrNodes = {};
		std::vector<Archive*> mDdsNodes = {};

		std::vector<ScrGroup> mScrGroups = {};
		std::vector<Texture2D*> mScrTextures = {};

	private:

		struct EntityData
		{
			bool Loaded;

			SceneInfo* SceneInfo;

			std::vector<Archive*> MdNodes = {};
			std::vector<Archive*> DdsNodes = {};

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
