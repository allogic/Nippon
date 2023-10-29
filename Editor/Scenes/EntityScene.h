#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Editor/Scene.h>

namespace ark
{
	namespace fs = std::filesystem;

	class FileContainer;
	class Archive;
	class Actor;

	class EntityScene : public Scene
	{
	public:

		EntityScene(const FileContainer* FileContainer);
		virtual ~EntityScene();

	public:

		inline const auto& GetEntityGeometryActor() const { return mEntityGeometryActor; }

	public:

		virtual void Load() override;
		virtual void Save() override;

	protected:

		virtual void AddResources() override;
		virtual void CreateAssets() override;
		virtual void BuildActors() override;

	private:

		Actor* mEntityGeometryActor = nullptr;
	};
}
