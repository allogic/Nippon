#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Actor::Actor(Scene* Scene, const std::string& Name)
    : mScene{ Scene }
    , mName{ Name }
    , mTransform{ AttachComponent<Transform>() }
  {

  }

  Actor::~Actor()
  {
    for (auto& [hash, component] : mComponents)
    {
      delete component;
      component = nullptr;
    }
  }

  void Actor::SetActive(bool Active)
  {
    mActive = Active;

    for (auto& child : mChildren)
    {
      child->SetActive(mActive);
    }
  }
}