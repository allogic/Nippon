#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Component.h>
#include <Editor/Mesh.h>
#include <Editor/Texture.h>
#include <Editor/Vertex.h>
#include <Editor/Math.h>

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
    inline const auto& GetAABB() const { return mAABB; }

  public:

    void SetVertexBuffer(const std::vector<DefaultVertex>& Vertices);
    void SetElementBuffer(const std::vector<U32>& Elements);
    void SetTexture(Texture2D* Texture);

  private:

    Mesh<DefaultVertex, U32> mMesh = {};
    Texture2D* mTexture = nullptr;

    AABB mAABB = {};
  };
}