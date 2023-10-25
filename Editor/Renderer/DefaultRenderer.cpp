#include <Editor/Mesh.h>
#include <Editor/Shader.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/Texture2D.h>
#include <Editor/Vertex.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Glad/glad.h>

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
		if (Scene* scene = SceneManager::GetActiveScene())
		{
			if (Camera* camera = scene->GetMainCamera())
			{
				mShader->Bind();

				mShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
				mShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());

				for (const auto& task : mRenderTasks)
				{
					mShader->SetUniformR32M4("UniformModelMatrix", task.Transform->GetModelMatrix());

					Mesh<DefaultVertex, U32>* mesh = task.Renderable->GetMesh();
					U32 texture = task.Renderable->GetTexture();

					if (texture)
					{
						Texture2D::Bind(texture);
						Texture2D::Mount(0, texture);
					}

					if (mesh)
					{
						mesh->Bind();
						mesh->Render(GL_TRIANGLES);
						mesh->Unbind();
					}

					if (texture)
					{
						Texture2D::UnMount(0);
						Texture2D::UnBind();
					}
				}

				mShader->UnBind();
			}
		}
	}

	void DefaultRenderer::FlushRenderTasks()
	{
		mRenderTasks.clear();
	}

	void DefaultRenderer::AddRenderTask(const RenderTask& RenderTask)
	{
		mRenderTasks.emplace_back(RenderTask);
	}
}
