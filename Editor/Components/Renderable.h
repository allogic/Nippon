#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Component.h>
#include <Editor/Mesh.h>
#include <Editor/Vertex.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Renderable : public Component
  {
  public:

    Renderable(Actor* Actor);

  public:

    inline auto GetMeshPtr() const { return &mMesh; }

  public:

    inline void SetVertexBuffer(const std::vector<DefaultVertex>& Value) { mMesh.UploadVertices(&Value[0], (U32)Value.size()); }
    inline void SetElementBuffer(const std::vector<U32>& Value) { mMesh.UploadElements(&Value[0], (U32)Value.size()); }

  private:

    Mesh<DefaultVertex, U32> mMesh = {};
  };
}