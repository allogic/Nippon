#include <filesystem>

#include <Editor/Mesh.h>
#include <Editor/Scene.h>
#include <Editor/Shader.h>
#include <Editor/Vertex.h>

#include <Editor/Components/Camera.h>

#include <Editor/Renderer/DebugRenderer.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::DebugRenderer* gDebugRenderer;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  namespace fs = std::filesystem;

  DebugRenderer::DebugRenderer(U32 VertexBufferSize, U32 ElementBufferSize)
    : mVertexBuffer{ new DebugVertex[VertexBufferSize] }
    , mElementBuffer{ new U32[ElementBufferSize] }
    , mMesh{ new Mesh<DebugVertex, U32> }
    , mShader{ new Shader{ fs::path{ SHADER_DIR } / "Debug.glsl" } }
  {

  }

  DebugRenderer::~DebugRenderer()
  {
    delete mShader;
    delete mMesh;
    delete[] mElementBuffer;
    delete[] mVertexBuffer;
  }

  void DebugRenderer::Render()
  {
    Camera* camera = Scene::GetMainCamera();

    if (camera)
    {
      mShader->Bind();

      mShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
      mShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());

      // Clear previous entries...

      mMesh->Bind();
      mMesh->UploadVertices(mVertexBuffer, mVertexOffset);
      mMesh->UploadElements(mElementBuffer, mElementOffset);
      mMesh->Render(RenderMode::eRenderModeLines);
      mMesh->UnBind();

      mShader->UnBind();
    }      

    mVertexOffset = 0;
    mElementOffset = 0;
  }

  void DebugRenderer::DebugLine(R32V3 P0, R32V3 P1, R32V4 C)
  {
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 0].Position = P0;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 1].Position = P1;

    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 0].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 1].Color = C;

    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 0] = gDebugRenderer->mVertexOffset + 0;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 1] = gDebugRenderer->mVertexOffset + 1;

    gDebugRenderer->mVertexOffset += 2;
    gDebugRenderer->mElementOffset += 2;
  }

  void DebugRenderer::DebugBox(R32V3 P, R32V3 S, R32V4 C, R32Q R)
  {
    R32V3 h{ S / 2.0F };

    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 0].Position = P + R * R32V3{ -h.x, -h.y, -h.z };
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 1].Position = P + R * R32V3{ h.x, -h.y, -h.z };
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 2].Position = P + R * R32V3{ -h.x, h.y, -h.z };
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 3].Position = P + R * R32V3{ h.x, h.y, -h.z };

    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 4].Position = P + R * R32V3{ -h.x, -h.y, h.z };
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 5].Position = P + R * R32V3{ h.x, -h.y, h.z };
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 6].Position = P + R * R32V3{ -h.x, h.y, h.z };
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 7].Position = P + R * R32V3{ h.x, h.y, h.z };

    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 0].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 1].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 2].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 3].Color = C;

    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 4].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 5].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 6].Color = C;
    gDebugRenderer->mVertexBuffer[gDebugRenderer->mVertexOffset + 7].Color = C;

    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 0] = gDebugRenderer->mVertexOffset + 0;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 1] = gDebugRenderer->mVertexOffset + 1;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 2] = gDebugRenderer->mVertexOffset + 0;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 3] = gDebugRenderer->mVertexOffset + 2;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 4] = gDebugRenderer->mVertexOffset + 2;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 5] = gDebugRenderer->mVertexOffset + 3;

    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 6] = gDebugRenderer->mVertexOffset + 3;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 7] = gDebugRenderer->mVertexOffset + 1;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 8] = gDebugRenderer->mVertexOffset + 4;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 9] = gDebugRenderer->mVertexOffset + 5;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 10] = gDebugRenderer->mVertexOffset + 4;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 11] = gDebugRenderer->mVertexOffset + 6;

    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 12] = gDebugRenderer->mVertexOffset + 6;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 13] = gDebugRenderer->mVertexOffset + 7;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 14] = gDebugRenderer->mVertexOffset + 7;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 15] = gDebugRenderer->mVertexOffset + 5;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 16] = gDebugRenderer->mVertexOffset + 0;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 17] = gDebugRenderer->mVertexOffset + 4;

    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 18] = gDebugRenderer->mVertexOffset + 1;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 19] = gDebugRenderer->mVertexOffset + 5;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 20] = gDebugRenderer->mVertexOffset + 2;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 21] = gDebugRenderer->mVertexOffset + 6;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 22] = gDebugRenderer->mVertexOffset + 3;
    gDebugRenderer->mElementBuffer[gDebugRenderer->mElementOffset + 23] = gDebugRenderer->mVertexOffset + 7;

    gDebugRenderer->mVertexOffset += 8;
    gDebugRenderer->mElementOffset += 24;
  }
}