#include <Editor/Mesh.h>
#include <Editor/Scene.h>
#include <Editor/Shader.h>
#include <Editor/Texture.h>
#include <Editor/Vertex.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Editor/Renderer/DefaultRenderer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  DefaultRenderer::DefaultRenderer(Scene* Scene)
    : mScene{ Scene }
    , mShader{ new Shader{ "Default.vert", "Default.frag" } }
  {

  }

  DefaultRenderer::~DefaultRenderer()
  {
    delete mShader;
  }

  void DefaultRenderer::Render()
  {
    while (!mRenderQueue.empty())
    {
      RenderTask& renderTask = mRenderQueue.front();

      if (renderTask.TransformPtr && renderTask.MeshPtr)
      {
        Camera* camera = mScene->GetMainCamera();

        if (camera)
        {
          mShader->Bind();

          mShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
          mShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());
          mShader->SetUniformR32M4("UniformModelMatrix", renderTask.TransformPtr->GetModelMatrix());

          if (renderTask.TexturePtr)
          {
            renderTask.TexturePtr->Mount(0);
          }

          renderTask.MeshPtr->Bind();
          renderTask.MeshPtr->Render(eRenderModeTriangles);
          renderTask.MeshPtr->Unbind();

          if (renderTask.TexturePtr)
          {
            renderTask.TexturePtr->UnMount();
          }

          mShader->Unbind();
        }
      }

      mRenderQueue.pop();
    }
  }

  void DefaultRenderer::AddRenderTask(const RenderTask& RenderTask)
  {
    mRenderQueue.emplace(RenderTask);
  }
}