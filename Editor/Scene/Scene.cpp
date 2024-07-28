#include <Common/Memory.h>

#include <Common/Archive/Archive.h>

#include <Common/Misc/BlowFish.h>

#include <Editor/Database/Database.h>

#include <Editor/Ecs/Registry.h>

#include <Editor/Glad/glad.h>

#include <Editor/Interface/Log.h>
#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/OpenGl/FrameBuffer.h>

#include <Editor/Renderer/Renderer.h>

#include <Editor/Scene/Scene.h>
#include <Editor/Scene/SceneAssets.h>
#include <Editor/Scene/SceneManager.h>

namespace Nippon
{
	Scene::Scene(ArchiveInfo const& ArchiveInfo)
	{
		mArchiveInfo = ArchiveInfo;

		CreateRegistry();
		CreateRenderer();
	}

	Scene::~Scene()
	{
		DestroyRenderer();
		DestroyRegistry();
		DestroyViewport();

		DestroySceneAssets();
	}

	void Scene::SetShouldBeDestroyed()
	{
		mShouldBeDestroyed = true;

		SceneManager::SetDirty();
	}

	void Scene::CreateRegistry()
	{
		mRegistry = new (Memory::Alloc(sizeof(Registry))) Registry{ this };
	}

	void Scene::CreateViewport()
	{
		mViewport = new (Memory::Alloc(sizeof(Viewport))) Viewport{ this };
	}

	void Scene::CreateRenderer()
	{
		mRenderer = new (Memory::Alloc(sizeof(Renderer))) Renderer{ this };
	}

	void Scene::DestroyRegistry()
	{
		mRegistry->~Registry();

		Memory::Free(mRegistry);
	}

	void Scene::DestroyViewport()
	{
		if (mViewport)
		{
			mViewport->~Viewport();

			Memory::Free(mViewport);
		}
	}

	void Scene::DestroyRenderer()
	{
		mRenderer->~Renderer();

		Memory::Free(mRenderer);
	}

	void Scene::AddArchiveByUniqueId(U32 UniqueId)
	{
		auto const& findIt = mSceneAssetsByUniqueId.find(UniqueId);

		if (findIt == mSceneAssetsByUniqueId.end())
		{
			Archive* archive = new (Memory::Alloc(sizeof(Archive))) Archive;
			SceneAssets* sceneAssets = new (Memory::Alloc(sizeof(SceneAssets))) SceneAssets;

			std::vector<U8> archiveData = Database::GetArchiveDataByUniqueId(UniqueId);

			archive->Deserialize(archiveData);

			sceneAssets->SetArchive(archive);

			mSceneAssetsByUniqueId[UniqueId] = sceneAssets;
		}
	}

	void Scene::DestroySceneAssets()
	{
		for (auto const& [uniqueId, sceneAssets] : mSceneAssetsByUniqueId)
		{
			sceneAssets->~SceneAssets();

			Memory::Free(sceneAssets);
		}
	}

	void Scene::Load()
	{

	}

	void Scene::Save()
	{

	}

	void Scene::Resize(U32 Width, U32 Height)
	{
		mWidth = Width;
		mHeight = Height;

		if (mFrameBuffer)
		{
			FrameBuffer::Destroy(mFrameBuffer);
		}

		mFrameBuffer = FrameBuffer::Create(mWidth, mHeight);
	}

	void Scene::FixedUpdate()
	{
		mRegistry->UpdateEntity(mRegistry->GetPlayerEntity(), true);
	}

	void Scene::Update()
	{
		if (mIsDirty)
		{
			mIsDirty = false;

			mRegistry->DestroyEntity(nullptr, true);
		}

		mRegistry->UpdateEntity(nullptr, true);
	}

	void Scene::Render()
	{
		if (mDebugEnabled)
		{
			mRenderer->DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1.0F, 0.0F, 0.0F }, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
			mRenderer->DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1.0F, 0.0F }, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
			mRenderer->DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1.0F }, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

			if (Entity* entity = Outline::GetSelectedEntity())
			{
				mRegistry->HandleEntitySelection(mRenderer, entity, true);
			}
		}

		if (mFrameBuffer)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer);

			glViewport(0, 0, (I32)mWidth, (I32)mHeight);

			glClearColor(0.125F, 0.125F, 0.125F, 1.0F);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthFunc(GL_LESS);

			mRenderer->RenderDefault();

			if (mDebugEnabled)
			{
				mRenderer->RenderDebug();
			}

			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
	}

	void Scene::Invalidate()
	{
		mRenderer->FlushDefaultQueue();

		Update();

		mRegistry->SubmitEntityToRenderer(mRenderer, nullptr, true);

		Render();
	}
}