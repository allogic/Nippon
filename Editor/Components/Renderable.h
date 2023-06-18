#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Component.h>
#include <Editor/Mesh.h>
#include <Editor/Texture.h>
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

    inline const auto& GetMesh() const { return mMesh; }
    inline const auto GetTexture() const { return mTexture; }

  public:

    inline void SetVertexBuffer(const std::vector<DefaultVertex>& Value) { mMesh.UploadVertices(&Value[0], (U32)Value.size()); }
    inline void SetElementBuffer(const std::vector<U32>& Value) { mMesh.UploadElements(&Value[0], (U32)Value.size()); }
    inline void SetTexture(Texture2D* Texture) { mTexture = Texture; }

  private:

    Mesh<DefaultVertex, U32> mMesh = {};
    Texture2D* mTexture = nullptr;
  };
}