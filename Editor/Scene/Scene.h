#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

#include <Editor/Database/Database.h>

namespace Nippon
{
	class Scene
	{
	public:

		Scene(ArchiveInfo const& ArchiveInfo);
		virtual ~Scene();

	public:

		inline auto const& GetUniqueId() const { return mArchiveInfo.UniqueId; }
		inline auto const& GetSceneType() const { return mArchiveInfo.ArchiveType; }
		inline auto const& GetFolderId() const { return mArchiveInfo.FolderId; }
		inline auto const& GetArchiveId() const { return mArchiveInfo.ArchiveId; }
		inline auto const& GetFolderName() const { return mArchiveInfo.FolderName; }
		inline auto const& GetSceneName() const { return mArchiveInfo.ArchiveName; }
		inline auto const& GetWindowName() const { return mArchiveInfo.WindowName; }
		inline auto const& GetFilePath() const { return mArchiveInfo.FilePath; }

		inline auto const& GetRegistry() const { return mRegistry; }
		inline auto const& GetViewport() const { return mViewport; }
		inline auto const& GetFrameBuffer() const { return mFrameBuffer; }
		inline auto const& GetWidth() const { return mWidth; }
		inline auto const& GetHeight() const { return mHeight; }
		inline auto const& GetShouldBeDestroyed() const { return mShouldBeDestroyed; }
		inline auto const& GetSceneAssetsByUniqueId(U32 UniqueId) const { return mSceneAssetsByUniqueId.at(UniqueId); }

		inline auto const GetAspectRatio() const { return (R32)mWidth / mHeight; }

	public:

		inline void SetDebugEnabled() { mDebugEnabled = true; }
		inline void SetDirty() { mIsDirty = true; }

		void SetShouldBeDestroyed();

	public:

		void CreateRegistry();
		void CreateViewport();
		void CreateRenderer();

		void DestroyRegistry();
		void DestroyViewport();
		void DestroyRenderer();

		void AddArchiveByUniqueId(U32 UniqueId);
		void DestroySceneAssets();

	public:

		virtual void Load();
		virtual void Save();

	public:

		void Resize(U32 Width, U32 Height);

		void FixedUpdate();
		void Update();
		void Render();

		void Invalidate();

	private:

		ArchiveInfo mArchiveInfo = {};

		Registry* mRegistry = nullptr;
		Viewport* mViewport = nullptr;
		Renderer* mRenderer = nullptr;

		std::map<U32, SceneAssets*> mSceneAssetsByUniqueId = {};

		U32 mWidth = 0;
		U32 mHeight = 0;
		U32 mFrameBuffer = 0;

		bool mDebugEnabled = false;
		bool mShouldBeDestroyed = false;
		bool mIsDirty = false;
	};
}