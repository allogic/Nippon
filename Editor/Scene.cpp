#include <Editor/Texture.h>
#include <Editor/Math.h>
#include <Editor/Scene.h>
#include <Editor/InterfaceManager.h>

#include <Editor/Actors/Player.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::R32 gTimeDelta;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	Scene::Scene(
		SceneType SceneType,
		const std::string& Entry,
		const std::string& SubEntry)
		: mSceneType{ SceneType }
		, mEntry{ Entry }
		, mSubEntry{ SubEntry }
		, mSceneName{ "" }
		, mWindowName{ "" }
		, mEnableDebug{ false }
	{
		mMainActor = CreateActor<Player>("Player", nullptr);
		mMainCamera = mMainActor->GetComponent<Camera>();
	}

	Scene::Scene(
		SceneType SceneType,
		const std::string& Entry,
		const std::string& SubEntry,
		const std::string& SceneName,
		const std::string& WindowName)
		: mSceneType{ SceneType }
		, mEntry{ Entry }
		, mSubEntry{ SubEntry }
		, mSceneName{ SceneName }
		, mWindowName{ WindowName }
		, mEnableDebug{ true }
	{
		mViewport = new Viewport{ this };

		mMainActor = CreateActor<Player>("Player", nullptr);
		mMainCamera = mMainActor->GetComponent<Camera>();
	}

	Scene::~Scene()
	{
		for (auto& actor : mActors)
		{
			delete actor;
			actor = nullptr;
		}

		for (auto& texture : mTextures)
		{
			delete texture;
			texture = nullptr;
		}

		if (mViewport)
		{
			delete mViewport;
			mViewport = nullptr;
		}
	}

	void Scene::DestroyActor(Actor* Actor)
	{
		auto actorIt = std::find(mActors.begin(), mActors.end(), Actor);

		if (actorIt != mActors.end())
		{
			delete* actorIt;
			*actorIt = nullptr;

			mActors.erase(actorIt);
		}
	}

	void Scene::Resize(U32 Width, U32 Height)
	{
		mWidth = Width;
		mHeight = Height;

		mFrameBuffer.Resize(mWidth, mHeight);
	}

	void Scene::Update()
	{
		if (mEnableDebug)
		{
			mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1000.0F, 0.0F, 0.0F }, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
			mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1000.0F, 0.0F }, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
			mDebugRenderer.DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1000.0F }, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });
		}

		for (const auto& actor : mActors)
		{
			actor->Update(gTimeDelta);
		}

		SubmitRenderTasks();

		if (mEnableDebug)
		{
			DoSelectionRecursive(InterfaceManager::GetOutline()->GetSelectedActor());
		}
	}

	void Scene::Render()
	{
		glViewport(0, 0, (I32)mWidth, (I32)mHeight);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer.GetId());

		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		mDefaultRenderer.Render();

		glDisable(GL_CULL_FACE);

		mDebugRenderer.Render();

		glDisable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void Scene::SubmitRenderTasks()
	{
		for (const auto& actor : mActors)
		{
			if (actor->IsActive())
			{
				Transform* transform = actor->GetTransform();
				Renderable* renderable = actor->GetComponent<Renderable>();

				if (transform && renderable)
				{
					mDefaultRenderer.AddRenderTask(RenderTask{ transform, &renderable->GetMesh(), renderable->GetTexture() });
				}
			}
		}
	}

	void Scene::DoSelectionRecursive(Actor* Actor)
	{
		if (Actor && Actor != mMainActor)
		{
			Transform* transform = Actor->GetComponent<Transform>();

			if (transform)
			{
				mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
				mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
				mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

				mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
				mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
				mDebugRenderer.DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

				mDebugRenderer.DebugAxisAlignedBoundingBox(Actor->GetAABB(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });

				for (const auto& child : Actor->GetChildren())
				{
					DoSelectionRecursive(child);
				}
			}
		}
	}

	std::vector<U8> Scene::Snapshot() const
	{
		std::vector<U8> bytes = {};

		bytes.resize(mWidth * mHeight * 4);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFrameBuffer.GetId());
		glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, &bytes[0]);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return bytes;
	}
}