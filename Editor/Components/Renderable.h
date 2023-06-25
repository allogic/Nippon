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

    Renderable(Scene* Scene, Actor* Actor);

  public:

    inline const auto& GetMesh() const { return mMesh; }
    inline const auto& GetTexture() const { return mTexture; }
    inline const auto& GetVertexBuffer() const { return mVertexBuffer; }
    inline const auto& GetElementBuffer() const { return mElementBuffer; }

  public:

    inline void SetVertexBuffer(const std::vector<DefaultVertex>& VertexBuffer) { mVertexBuffer = VertexBuffer; }
    inline void SetElementBuffer(const std::vector<U32>& ElementBuffer) { mElementBuffer = ElementBuffer; }
    inline void SetTexture(Texture2D* Texture) { mTexture = Texture; }

  public:

    void LocalToRemote();
    void RemoteToLocal();

  private:

    std::vector<DefaultVertex> mVertexBuffer = {};
    std::vector<U32> mElementBuffer = {};

    Mesh<DefaultVertex, U32> mMesh = {};
    Texture2D* mTexture = nullptr;
  };
}