#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <utility>

#include <Common/Types.h>

#include <Editor/Actor.h>
#include <Editor/GenericModel.h>
#include <Editor/GenericTexture.h>

#include <Editor/Databases/FileDatabase.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Viewport;
	class Camera;
	class GenericModel;

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
		inline const auto& GetModels() const { return mModels; }
		inline const auto& GetTextures() const { return mTextures; }

	public:

		inline void SetDirty(bool Value) { mIsDirty = Value; }
		inline void SetEnableConsole(bool Value) { mEnableConsole = Value; }
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

		GenericModel& AddModel();
		GenericTexture& AddTexture();

	protected:

		bool mEnableConsole = false;
		bool mEnableDebug = false;

	private:

		const FileContainer* mFileContainer;

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

		std::vector<GenericModel> mModels = {};
		std::vector<GenericTexture> mTextures = {};
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
