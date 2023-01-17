#pragma once

#include <Common/Types.h>

#include <Editor/UI.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Actor;

  class SceneOutline : public UI
  {
  public:

    virtual void Update() override;
    virtual void Draw() override;

  private:

    void DrawActorRecursive(Actor* Actor);

  private:

    Actor* mSelectedActor = nullptr;
  };
}