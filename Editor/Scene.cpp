#include <Editor/Editor.h>
#include <Editor/Texture.h>
#include <Editor/InterfaceManager.h>
#include <Editor/SceneManager.h>
#include <Editor/Scene.h>

#include <Editor/Actors/Player.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	Scene::Scene(const SceneInfo& Info)
		: mSceneInfo{ Info }
	{
		mFrameBuffer = new FrameBuffer
		{
			{
				AttachmentDescription{ GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGBA, GL_RGBA32F, GL_FLOAT },
				AttachmentDescription{ GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT },
			},
			AttachmentDescription{ GL_CLAMP_TO_EDGE, GL_LINEAR, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8 }
		};

		mRootActor = CreateActor<Actor>("Root", nullptr);
		mPlayerActor = CreateActor<Player>("Player", nullptr);

		mMainCamera = mPlayerActor->GetComponent<Camera>();
	}

	Scene::~Scene()
	{
		DestroyActorRecursive();

		if (mFrameBuffer)
		{
			delete mFrameBuffer;
			mFrameBuffer = nullptr;
		}

		if (mViewport)
		{
			delete mViewport;
			mViewport = nullptr;
		}
	}

	void Scene::CreateViewport()
	{
		 if (!mViewport)
		 {
			mViewport = new Viewport{ this };
		 }
	}

	void Scene::MakeShouldBeDestroyed(bool Value)
	{
		mShouldBeDestroyed = Value;

		SceneManager::SetDirty(true);
	}

	void Scene::Resize(U32 Width, U32 Height)
	{
		mWidth = Width;
		mHeight = Height;

		mFrameBuffer->Resize(mWidth, mHeight);
	}

	void Scene::PreRender()
	{
		SubmitActorToRendererRecursive();

		if (mEnableDebug)
		{
			gDebugRenderer->DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1.0F, 0.0F, 0.0F }, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
			gDebugRenderer->DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1.0F, 0.0F }, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
			gDebugRenderer->DebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1.0F }, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

			if (Actor* actor = InterfaceManager::GetOutline()->GetSelectedActor())
			{
				HandleActorSelectionRecursive(actor);
			}
		}
	}

	void Scene::Render()
	{
		glViewport(0, 0, (I32)mWidth, (I32)mHeight);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer->GetId());

		glClearColor(0.125F, 0.125F, 0.125F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// TODO: https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		gDefaultRenderer->Render();

		glDisable(GL_CULL_FACE);

		gDebugRenderer->Render();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void Scene::PostRender()
	{
		// TODO
	}

	void Scene::PreUpdate()
	{
		// TODO
	}

	void Scene::Update()
	{
		UpdateActorRecursive();
	}

	void Scene::PostUpdate()
	{
		if (mIsDirty)
		{
			mIsDirty = false;

			DestroyActorIfMarkedRecursive();
			Step();
		}
	}

	void Scene::Step()
	{
		PreRender();
		Render();
		PostRender();

		PreUpdate();
		Update();
		PostUpdate();
	}

	void Scene::UpdateActorRecursive(Actor* Actor)
	{
		if (!Actor)
		{
			Actor = mRootActor;
		}

		if (Actor)
		{
			for (const auto& child : Actor->GetChildren())
			{
				UpdateActorRecursive(child);
			}

			Actor->Update();
		}
	}

	bool Scene::DestroyActorRecursive(Actor* Actor)
	{
		if (!Actor)
		{
			Actor = mRootActor;
		}

		if (Actor)
		{
			auto& children = Actor->GetChildren();
			for (auto it = children.begin(); it != children.end();)
			{
				if (DestroyActorRecursive(*it))
				{
					it = children.erase(it);
				}
				else
				{
					it++;
				}
			}

			bool removeFromParent = Actor->GetParent();

			if (Actor == mRootActor)
			{
				mRootActor = nullptr;
			}

			if (Actor == mPlayerActor)
			{
				mMainCamera = nullptr;
			}

			if (Actor == InterfaceManager::GetOutline()->GetSelectedActor())
			{
				InterfaceManager::GetOutline()->SetSelectedActor(nullptr);
			}

			delete Actor;

			return removeFromParent;
		}

		return false;
	}

	bool Scene::DestroyActorIfMarkedRecursive(Actor* Actor)
	{
		if (!Actor)
		{
			Actor = mRootActor;
		}

		if (Actor)
		{
			auto& children = Actor->GetChildren();
			for (auto it = children.begin(); it != children.end();)
			{
				if (DestroyActorIfMarkedRecursive(*it))
				{
					it = children.erase(it);
				}
				else
				{
					it++;
				}
			}

			if (Actor->GetShouldBeDestroyed())
			{
				bool removeFromParent = Actor->GetParent();

				if (Actor == mRootActor)
				{
					mRootActor = nullptr;
				}

				if (Actor == mPlayerActor)
				{
					mMainCamera = nullptr;
				}

				if (Actor == InterfaceManager::GetOutline()->GetSelectedActor())
				{
					InterfaceManager::GetOutline()->SetSelectedActor(nullptr);
				}

				delete Actor;

				return removeFromParent;
			}
		}

		return false;
	}

	Actor* Scene::FindActorByIdRecursive(U32 Id, Actor* Actor)
	{
		if (!Actor)
		{
			Actor = mRootActor;
		}

		if (Actor)
		{
			for (const auto& child : Actor->GetChildren())
			{
				if (const auto actor = FindActorByIdRecursive(Id, child))
				{
					return actor;
				}
			}

			if (Id == Actor->GetId())
			{
				return Actor;
			}
		}

		return nullptr;
	}

	void Scene::SubmitActorToRendererRecursive(Actor* Actor)
	{
		if (!Actor)
		{
			Actor = mRootActor;
		}

		if (Actor && Actor->IsActive())
		{
			for (const auto& child : Actor->GetChildren())
			{
				SubmitActorToRendererRecursive(child);
			}

			Transform* transform = Actor->GetTransform();
			Renderable* renderable = Actor->GetComponent<Renderable>();

			if (transform && renderable)
			{
				gDefaultRenderer->AddToRenderQueue(Actor);
			}
		}
	}

	void Scene::HandleActorSelectionRecursive(Actor* Actor)
	{
		if (!Actor)
		{
			Actor = mRootActor;
		}

		if (Actor && Actor->IsActive())
		{
			for (const auto& child : Actor->GetChildren())
			{
				HandleActorSelectionRecursive(child);
			}

			Transform* transform = Actor->GetComponent<Transform>();

			if (transform)
			{
				gDebugRenderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
				gDebugRenderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
				gDebugRenderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetWorldFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

				gDebugRenderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalRight(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
				gDebugRenderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalUp(), R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
				gDebugRenderer->DebugLine(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalFront(), R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

				gDebugRenderer->DebugAABB(transform->GetWorldPosition(), Actor->GetAABB(), R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
			}
		}
	}

	std::vector<U8> Scene::Snapshot(U8 Channels, U32 Type) const
	{
		std::vector<U8> bytes = {};

		bytes.resize(mWidth * mHeight * Channels);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFrameBuffer->GetId());
		glReadPixels(0, 0, mWidth, mHeight, mFrameBuffer->GetColorTexture(0)->GetFormat(), Type, &bytes[0]);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return bytes;
	}
}
