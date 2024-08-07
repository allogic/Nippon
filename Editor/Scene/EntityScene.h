#pragma once

#include <Editor/Forward.h>

#include <Editor/Scene/Scene.h>

namespace Nippon
{
	class EntityScene : public Scene
	{
	public:

		EntityScene(ArchiveInfo const& ArchiveInfo, bool LoadFromFilePath);
		virtual ~EntityScene();

	public:

		void Load() override;
		void Save() override;

	private:

		void AddResources();
		void CreateAssets();
		void BuildEntities();

	private:

		Entity* mStaticGeometryEntity = nullptr;
	};
}