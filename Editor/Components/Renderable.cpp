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

  void Renderable::LocalToRemote()
  {
    mMesh.SetVertices(&mVertexBuffer[0], (U32)mVertexBuffer.size());
    mMesh.SetElements(&mElementBuffer[0], (U32)mElementBuffer.size());
  }

  void Renderable::RemoteToLocal()
  {
    mMesh.GetVertices(&mVertexBuffer[0], (U32)mVertexBuffer.size());
    mMesh.GetElements(&mElementBuffer[0], (U32)mElementBuffer.size());
  }
}