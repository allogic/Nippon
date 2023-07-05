#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <utility>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Actor.h>
#include <Editor/Header.h>
#include <Editor/Model.h>
#include <Editor/FrameBuffer.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	enum SceneType
	{
		eSceneTypeLevel,
		eSceneTypeEntity,
	};

	class Scene
	{
	public:

		Scene(
			SceneType SceneType,
			const std::string& Entry,
			const std::string& SubEntry);
		Scene(
			SceneType SceneType,
			const std::string& Entry,
			const std::string& SubEntry,
			const std::string& SceneName,
			const std::string& WindowName);
		virtual ~Scene();

	public:

		inline const auto& GetSceneType() const { return mSceneType; }
		inline const auto& GetSceneName() const { return mSceneName; }
		inline const auto& GetWindowName() const { return mWindowName; }
		inline const auto& GetWidth() const { return mWidth; }
		inline const auto& GetHeight() const { return mHeight; }
		inline const auto& GetActors() const { return mActors; }
		inline const auto& GetFrameBuffer() const { return mFrameBuffer; }
		inline const auto& GetMainActor() const { return mMainActor; }
		inline const auto& GetStaticGeometryActor() const { return mStaticGeometryActor; }
		inline const auto& GetMainCamera() const { return mMainCamera; }
		inline const auto& GetViewport() const { return mViewport; }

		inline auto& GetDebugRenderer() { return mDebugRenderer; }
		inline auto& GetDefaultRenderer() { return mDefaultRenderer; }

	public:

		template<typename A, typename ... Args>
		A* CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments);

		void DestroyActor(Actor* Actor);

	public:

		void Resize(U32 Width, U32 Height);
		void Update();
		void Render();

		void SubmitRenderTasks();
		void DoSelectionRecursive(Actor* Actor);

		std::vector<U8> Snapshot() const;

	protected:

		virtual void Load() = 0;
		virtual void Save() = 0;

	protected:

		SceneType mSceneType;
		std::string mEntry;
		std::string mSubEntry;
		std::string mSceneName;
		std::string mWindowName;
		bool mEnableDebug;

		Viewport* mViewport = nullptr;

		Actor* mMainActor = nullptr;
		Actor* mStaticGeometryActor = nullptr;

		Camera* mMainCamera = nullptr;

		std::vector<Actor*> mActors = {};

		U32 mWidth = 1;
		U32 mHeight = 1;

		DebugRenderer mDebugRenderer = { this, 65535, 65535 * 2 };
		DefaultRenderer mDefaultRenderer = { this };

		FrameBuffer mFrameBuffer = {};
	};
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	template<typename A, typename ... Args>
	A* Scene::CreateActor(const std::string& Name, Actor* Parent, Args&& ... Arguments)
	{
		auto actor = mActors.emplace_back(new A{ this, Name, std::forward<Args>(Arguments) ... });
		if (Parent)
		{
			actor->SetParent(Parent);
			Parent->AddChild(actor);
		}
		return (A*)actor;
	}
}