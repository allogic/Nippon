#include <Editor/Component.h>
#include <Editor/Scene.h>
#include <Editor/Actor.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Component::Component(Scene* Scene, Actor* Actor)
    : mScene{ Scene }
    , mActor{ Actor }
  {

  }

  Component::~Component()
  {

  }
}