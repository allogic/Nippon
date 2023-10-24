#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Editor/Scene.h>

#include <Editor/Structs/MdStructs.h>

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
		inline const auto& GetMdGroups() const { return mMdGroups; }
		inline const auto& GetMdTextures() const { return mMdTextures; }

	public:

		virtual void Load() override;
		virtual void Save() override;

	private:

		void LoadArchives();
		void LoadEntity();

	private:

		void AddStaticGeometry();

	private:

		void PrintSummary();

	private:

		Actor* mEntityGeometryActor = nullptr;

	private:

		Archive* mDatArchive = nullptr;

	private:

		std::vector<Archive*> mMdArchives;
		std::vector<Archive*> mDdsArchives;

	private:

		std::vector<MdGroup> mMdGroups = {};
		std::vector<U32> mMdTextures = {};
	};
}
