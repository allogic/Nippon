#pragma once

#include <Common/Structs/PlacementStructs.h>

#include <Editor/Forward.h>

#include <Editor/Scene/Scene.h>

namespace Nippon
{
	class LevelScene : public Scene
	{
	public:

		LevelScene(ArchiveInfo const& ArchiveInfo, bool LoadFromFilePath);
		virtual ~LevelScene();

	public:

		void Load() override;
		void Save() override;

	private:

		void AddResources();
		void CreateAssets();
		void BuildEntities();

	private:

		void AddEntityResources(std::vector<Object> const& Objects);
		void CreateEntityAssets(std::vector<Object> const& Objects);
		void BuildEntityEntities(std::vector<Object> const& Objects);

	private:

		Entity* mStaticGeometryEntity = nullptr;

		std::vector<Object> mTscObjects = {};
		std::vector<Object> mTreObjects = {};
		std::vector<Object> mTatObjects = {};
	};
}