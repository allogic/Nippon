#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <utility>

#include <Common/Types.h>

#include <Editor/Actor.h>
#include <Editor/Model.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Archive;
	class Viewport;
	class Camera;
	class FileContainer;
	class FileInfo;

	class SceneResource
	{
	public:

		friend class Scene;

	public:

		inline const auto& IsLoaded() const { return mLoaded; }

		inline const auto& GetArchive() const { return mArchive; }
		inline const auto& GetModels() const { return mModels; }
		inline const auto& GetTextures() const { return mTextures; }

	public:

		inline auto GetTexturesByIndex(U32 Index) const { return (Index > mTextures.size()) ? 0 : mTextures[Index]; }

	public:

		inline void SetLoaded(bool Value) { mLoaded = Value; }

	public:

		void AddScrModelFromArchive(Archive* Archive);
		void AddMdModelFromArchive(Archive* Archive, const Object* Object);
		void AddTextureFromArchive(Archive* Archive);

	private:

		Archive* mArchive = {};

		bool mLoaded = false;

		std::vector<Model> mModels = {};
		std::vector<U32> mTextures = {};
	};

	class Scene
	{
	public:

		Scene(const FileContainer* FileContainer);
		virtual ~Scene();

	public:

		inline const auto& GetFileContainer() const { return mFileContainer; }
		inline const auto& GetWidth() const { return mWidth; }
		inline const auto& GetHeight() const { return mHeight; }
		inline const auto& GetFrameBuffer() const { return mFrameBuffer; }
		inline const auto& GetRootActor() const { return mRootActor; }
		inline const auto& GetPlayerActor() const { return mPlayerActor; }
		inline const auto& GetMainCamera() const { return mMainCamera; }
		inline const auto& GetViewport() const { return mViewport; }
		inline const auto& GetShouldBeDestroyed() const { return mShouldBeDestroyed; }

		inline const auto GetSceneResourceByIdentifier(U32 Identifier) { return mSceneResourcesByIdentifier[Identifier]; }

	public:

		inline void SetDirty(bool Value) { mIsDirty = Value; }
		inline void SetEnableDebug(bool Value) { mEnableDebug = Value; }

	public:

		void CreateViewport();
		void MakeShouldBeDestroyed(bool Value);

	public:

		template<typename A, typename ... Args>
		A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

	public:

		void Resize(U32 Width, U32 Height);

		void Update();
		void Render();

		void Invalidate();

	public:

		void UpdateActorRecursive(Actor* Actor = nullptr);
		bool DestroyActorRecursive(Actor* Actor = nullptr);
		bool DestroyActorIfMarkedRecursive(Actor* Actor = nullptr);
		Actor* FindActorByIdRecursive(U32 Id, Actor* Actor = nullptr);
		void SubmitActorToRendererRecursive(Actor* Actor = nullptr);
		void HandleActorSelectionRecursive(Actor* Actor = nullptr);

	public:

		virtual void Load() = 0;
		virtual void Save() = 0;

	protected:

		virtual void AddResources() = 0;
		virtual void CreateAssets() = 0;
		virtual void BuildActors() = 0;

	protected:

		Archive* AddResource(U32 Identifier, const FileInfo* FileInfo);

	protected:

		bool mEnableDebug = false;

	private:

		const FileContainer* mFileContainer;

		std::map<U32, SceneResource*> mSceneResourcesByIdentifier = {};

		Viewport* mViewport = nullptr;

		U32 mUniqueActorId = 0;

		Actor* mRootActor = nullptr;
		Actor* mPlayerActor = nullptr;

		Camera* mMainCamera = nullptr;

		U32 mWidth = 0;
		U32 mHeight = 0;

		U32 mFrameBuffer = 0;

		bool mIsDirty = false;
		bool mShouldBeDestroyed = false;
	};
}

namespace ark
{
	template<typename A, typename ... Args>
	A* Scene::CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments)
	{
		Actor* parent = Parent ? Parent : mRootActor;
		Actor* actor = new A{ this, mUniqueActorId++, Name, std::forward<Args>(Arguments) ... };

		actor->SetParent(parent);

		if (parent)
		{
			parent->AddChild(actor);
		}

		return (A*)actor;
	}
}
