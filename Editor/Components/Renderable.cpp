#include <Editor/Actor.h>
#include <Editor/Mesh.h>

#include <Editor/Components/Renderable.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Renderable::Renderable(Actor* Actor)
    : Component{ Actor }
  {

  }
}