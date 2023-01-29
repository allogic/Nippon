#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class SceneOutline : public Interface
  {
  public:

    virtual void Draw() override;

  private:

    void DrawActorRecursive(Actor* Actor);

  private:

    Actor* mSelectedActor = nullptr;
  };
}