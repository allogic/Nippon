#include <Editor/Mesh.h>
#include <Editor/Shader.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/Texture.h>
#include <Editor/Vertex.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Renderer/DefaultRenderer.h>

namespace ark
{
	DefaultRenderer::DefaultRenderer()
		: mShader{ new Shader{ "Default.vert", "Default.frag" } }
	{

	}

	DefaultRenderer::~DefaultRenderer()
	{
		delete mShader;
	}

	void DefaultRenderer::Render()
	{
		Camera* camera = SceneManager::GetActiveScene()->GetMainCamera();

		if (camera)
		{
			mShader->Bind();

			mShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
			mShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());

			while (!mRenderQueue.empty())
			{
				Actor* actor = mRenderQueue.front();

				Transform* transform = actor->GetTransform();
				Renderable* renderable = actor->GetComponent<Renderable>();

				mShader->SetUniformR32M4("UniformModelMatrix", transform->GetModelMatrix());

				if (Texture2D* texture = renderable->GetTexture())
				{
					texture->Bind();
					texture->Mount(0);
				}

				if (Mesh<DefaultVertex, U32>* mesh = renderable->GetMesh())
				{
					mesh->Bind();
					mesh->Render(eRenderModeTriangles);
					mesh->Unbind();
				}

				if (Texture2D* texture = renderable->GetTexture())
				{
					texture->UnMount();
					texture->UnBind();
				}

				mRenderQueue.pop();
			}

			mShader->UnBind();
		}
	}

	void DefaultRenderer::AddToRenderQueue(Actor* Actor)
	{
		mRenderQueue.emplace(Actor);
	}
}
