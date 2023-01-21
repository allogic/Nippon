#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class DebugRenderer
  {
  public:

    DebugRenderer(U32 VertexBufferSize, U32 ElementBufferSize);
    virtual ~DebugRenderer();

  public:

    void Render();

  public:

    static void DebugLine(R32V3 P0, R32V3 P1, R32V4 C);
    static void DebugBox(R32V3 P, R32V3 S, R32V4 C, R32Q R = {});

  private:

    DebugVertex* mVertexBuffer;
    U32* mElementBuffer;
    Mesh<DebugVertex, U32>* mMesh;
    Shader* mShader;

    U32 mVertexOffset = 0;
    U32 mElementOffset = 0;
  };
}