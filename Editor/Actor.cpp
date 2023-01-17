#include <Editor/Actor.h>

#include <Editor/Components/Transform.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Actor::Actor(const std::string& Name)
    : mName{ Name }
    , mTransform{ ComponentAttach<Transform>() }
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
}