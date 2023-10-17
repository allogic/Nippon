#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Common/Types.h>

#include <Editor/Scene.h>

#include <Editor/Generated/SceneInfos.h>

#include <Editor/Structs/MdStructs.h>

namespace ark
{
	namespace fs = std::filesystem;

	class BlowFish;
	class Archive;
	class Texture2D;

	class EntityScene : public Scene
	{
	public:

		EntityScene(const SceneInfo& Info);
		virtual ~EntityScene();

	public:

		inline const auto& GetEntityGeometryActor() const { return mEntityGeometryActor; }
		inline const auto& GetMdGroups() const { return mMdGroups; }
		inline const auto& GetMdTextures() const { return mMdTextures; }

	public:

		virtual void Load() override;
		virtual void Save() override;

	private:

		void LoadArchive();
		void LoadEntity();

	private:

		void AddStaticGeometry();

	private:

		void PrintSummary();

	private:

		Actor* mEntityGeometryActor = nullptr;

	private:

		BlowFish* mCipher = nullptr;

		Archive* mDatArchive = nullptr;

	private:

		std::vector<Archive*> mMdNodes;
		std::vector<Archive*> mDdsNodes;

	private:

		std::vector<MdGroup> mMdGroups = {};
		std::vector<Texture2D*> mMdTextures = {};
	};
}
