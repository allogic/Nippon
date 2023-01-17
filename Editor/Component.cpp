#include <Editor/Component.h>
#include <Editor/Actor.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Component::Component(Actor* Actor)
    : mActor{ Actor }
  {

  }

  Component::~Component()
  {

  }
}