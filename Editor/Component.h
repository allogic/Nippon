#pragma once

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Actor;

  class Component
  {
  public:

    Component(Actor* Actor);
    virtual ~Component();

  protected:

    Actor* mActor;
  };
}