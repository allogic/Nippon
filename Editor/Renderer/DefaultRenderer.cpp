#include <Editor/Mesh.h>
#include <Editor/Scene.h>
#include <Editor/Shader.h>
#include <Editor/Vertex.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Editor/Renderer/DefaultRenderer.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

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
    while (!mRenderQueue.empty())
    {
      RenderTask& renderTask = mRenderQueue.front();

      if (renderTask.TransformPtr && renderTask.MeshPtr)
      {
        if (gScene)
        {
          Camera* camera = gScene->GetMainCamera();

          if (camera)
          {
            mShader->Bind();

            mShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
            mShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());
            mShader->SetUniformR32M4("UniformModelMatrix", renderTask.TransformPtr->GetModelMatrix());

            renderTask.MeshPtr->Bind();
            renderTask.MeshPtr->Render(eRenderModeTriangles);
            renderTask.MeshPtr->Unbind();

            mShader->Unbind();
          }
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