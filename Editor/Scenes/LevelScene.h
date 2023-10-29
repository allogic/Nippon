#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <set>
#include <map>

#include <Common/Types.h>

#include <Editor/Scene.h>

#include <Editor/Structs/ObjectStructs.h>

namespace ark
{
	namespace fs = std::filesystem;

	class FileContainer;
	class Archive;
	class Actor;

	class LevelScene : public Scene
	{
	public:

		LevelScene(const FileContainer* FileContainer);
		virtual ~LevelScene();

	public:

		inline const auto& GetLevelGeometryActor() const { return mLevelGeometryActor; }
		inline const auto& GetEntityGeometryActor() const { return mEntityGeometryActor; }

	public:

		virtual void Load() override;
		virtual void Save() override;

	protected:

		virtual void AddResources() override;
		virtual void CreateAssets() override;
		virtual void BuildActors() override;

	private:

		void AddEntityResources(const std::vector<Object>& Objects);
		void CreateEntityAssets(const std::vector<Object>& Objects);
		void BuildEntityActors(const std::vector<Object>& Objects);

	private:

		Actor* mLevelGeometryActor = nullptr;
		Actor* mEntityGeometryActor = nullptr;

	private:

		std::vector<Object> mTscObjects = {};
		std::vector<Object> mTreObjects = {};
		std::vector<Object> mTatObjects = {};

	private:

		std::map<U16, std::set<U8>> mTmp = {};
	};
}
