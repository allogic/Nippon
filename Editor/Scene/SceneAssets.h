#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Archive/Model.h>

#include <Common/Archive/Structs/MeshStructs.h>
#include <Common/Archive/Structs/PlacementStructs.h>

#include <Editor/Forward.h>

namespace Nippon
{
	class SceneAssets
	{
	public:

		SceneAssets();
		virtual ~SceneAssets();

	public:

		inline auto const& IsLoaded() const { return mIsLoaded; }

		inline auto const& GetArchive() const { return mArchive; }
		inline auto const& GetModels() const { return mModels; }
		inline auto const& GetTextures() const { return mTextures; }

		inline auto const GetTextureByIndex(U32 Index) const { return (Index < mTextures.size()) ? mTextures[Index] : 0; }

	public:

		inline void SetArchive(Archive* Archive) { mArchive = Archive; }
		inline void SetLoaded() { mIsLoaded = true; }

	public:

		void AddScrModelFromArchive(Archive* Archive);
		void AddMdModelFromArchive(Archive* Archive, Object const* Object);
		void AddTextureFromArchive(Archive* Archive);

	private:

		Archive* mArchive = nullptr;

		std::vector<Model> mModels = {};
		std::vector<U32> mTextures = {};

		bool mIsLoaded = false;
	};
}