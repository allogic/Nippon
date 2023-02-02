#pragma once

#include <filesystem>

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

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

    void DebugLine(R32V3 P0, R32V3 P1, R32V4 C);
    void DebugBox(R32V3 P, R32V3 S, R32V4 C, R32Q R = {});

  private:

    DebugVertex* mVertexBuffer;
    U32* mElementBuffer;
    Mesh<DebugVertex, U32>* mMesh;
    Shader* mShader;

    U32 mVertexOffset = 0;
    U32 mElementOffset = 0;
  };
}