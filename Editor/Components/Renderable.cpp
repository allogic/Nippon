#include <Editor/Actor.h>
#include <Editor/Mesh.h>

#include <Editor/Components/Renderable.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Renderable::Renderable(Scene* Scene, Actor* Actor)
    : Component{ Scene, Actor }
  {

  }

  void Renderable::SetVertexBuffer(const std::vector<DefaultVertex>& Verties)
  {
    mMesh.UploadVertices(&Verties[0], (U32)Verties.size());
  }

  void Renderable::SetElementBuffer(const std::vector<U32>& Elements)
  {
    mMesh.UploadElements(&Elements[0], (U32)Elements.size());
  }

  void Renderable::SetTexture(Texture2D* Texture)
  {
    mTexture = Texture;
  }
}