#pragma once

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Component
  {
  public:

    Component(Actor* Actor);
    virtual ~Component();

  protected:

    Actor* mActor;
  };
}