#include <Editor/Mesh.h>
#include <Editor/Scene.h>
#include <Editor/Shader.h>
#include <Editor/Vertex.h>

#include <Editor/Components/Camera.h>

#include <Editor/Renderer/DebugRenderer.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  DebugRenderer::DebugRenderer(U32 VertexBufferSize, U32 ElementBufferSize)
    : mVertexBuffer{ new DebugVertex[VertexBufferSize] }
    , mElementBuffer{ new U32[ElementBufferSize] }
    , mMesh{ new Mesh<DebugVertex, U32> }
    , mShader{ new Shader{ "Debug.vert", "Debug.frag" } }
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
    if (gScene)
    {
      Camera* camera = gScene->GetMainCamera();

      if (camera)
      {
        mShader->Bind();

        mShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
        mShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());

        // Clear previous entries...

        mMesh->Bind();
        mMesh->UploadVertices(mVertexBuffer, mVertexOffset);
        mMesh->UploadElements(mElementBuffer, mElementOffset);
        mMesh->Render(eRenderModeLines);
        mMesh->Unbind();

        mShader->Unbind();
      }
    }

    mVertexOffset = 0;
    mElementOffset = 0;
  }

  void DebugRenderer::DebugLine(R32V3 P0, R32V3 P1, R32V4 C)
  {
    mVertexBuffer[mVertexOffset + 0].Position = P0;
    mVertexBuffer[mVertexOffset + 1].Position = P1;

    mVertexBuffer[mVertexOffset + 0].Color = C;
    mVertexBuffer[mVertexOffset + 1].Color = C;

    mElementBuffer[mElementOffset + 0] = mVertexOffset + 0;
    mElementBuffer[mElementOffset + 1] = mVertexOffset + 1;

    mVertexOffset += 2;
    mElementOffset += 2;
  }

  void DebugRenderer::DebugBox(R32V3 P, R32V3 S, R32V4 C, R32Q R)
  {
    R32V3 h{ S / 2.0F };

    mVertexBuffer[mVertexOffset + 0].Position = P + R * R32V3{ -h.x, -h.y, -h.z };
    mVertexBuffer[mVertexOffset + 1].Position = P + R * R32V3{ h.x, -h.y, -h.z };
    mVertexBuffer[mVertexOffset + 2].Position = P + R * R32V3{ -h.x, h.y, -h.z };
    mVertexBuffer[mVertexOffset + 3].Position = P + R * R32V3{ h.x, h.y, -h.z };

    mVertexBuffer[mVertexOffset + 4].Position = P + R * R32V3{ -h.x, -h.y, h.z };
    mVertexBuffer[mVertexOffset + 5].Position = P + R * R32V3{ h.x, -h.y, h.z };
    mVertexBuffer[mVertexOffset + 6].Position = P + R * R32V3{ -h.x, h.y, h.z };
    mVertexBuffer[mVertexOffset + 7].Position = P + R * R32V3{ h.x, h.y, h.z };

    mVertexBuffer[mVertexOffset + 0].Color = C;
    mVertexBuffer[mVertexOffset + 1].Color = C;
    mVertexBuffer[mVertexOffset + 2].Color = C;
    mVertexBuffer[mVertexOffset + 3].Color = C;

    mVertexBuffer[mVertexOffset + 4].Color = C;
    mVertexBuffer[mVertexOffset + 5].Color = C;
    mVertexBuffer[mVertexOffset + 6].Color = C;
    mVertexBuffer[mVertexOffset + 7].Color = C;

    mElementBuffer[mElementOffset + 0] = mVertexOffset + 0;
    mElementBuffer[mElementOffset + 1] = mVertexOffset + 1;
    mElementBuffer[mElementOffset + 2] = mVertexOffset + 0;
    mElementBuffer[mElementOffset + 3] = mVertexOffset + 2;
    mElementBuffer[mElementOffset + 4] = mVertexOffset + 2;
    mElementBuffer[mElementOffset + 5] = mVertexOffset + 3;

    mElementBuffer[mElementOffset + 6] = mVertexOffset + 3;
    mElementBuffer[mElementOffset + 7] = mVertexOffset + 1;
    mElementBuffer[mElementOffset + 8] = mVertexOffset + 4;
    mElementBuffer[mElementOffset + 9] = mVertexOffset + 5;
    mElementBuffer[mElementOffset + 10] = mVertexOffset + 4;
    mElementBuffer[mElementOffset + 11] = mVertexOffset + 6;

    mElementBuffer[mElementOffset + 12] = mVertexOffset + 6;
    mElementBuffer[mElementOffset + 13] = mVertexOffset + 7;
    mElementBuffer[mElementOffset + 14] = mVertexOffset + 7;
    mElementBuffer[mElementOffset + 15] = mVertexOffset + 5;
    mElementBuffer[mElementOffset + 16] = mVertexOffset + 0;
    mElementBuffer[mElementOffset + 17] = mVertexOffset + 4;

    mElementBuffer[mElementOffset + 18] = mVertexOffset + 1;
    mElementBuffer[mElementOffset + 19] = mVertexOffset + 5;
    mElementBuffer[mElementOffset + 20] = mVertexOffset + 2;
    mElementBuffer[mElementOffset + 21] = mVertexOffset + 6;
    mElementBuffer[mElementOffset + 22] = mVertexOffset + 3;
    mElementBuffer[mElementOffset + 23] = mVertexOffset + 7;

    mVertexOffset += 8;
    mElementOffset += 24;
  }
}