#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Outline : public Interface
  {
  public:

    virtual void Reset() override;
    virtual void Draw() override;

  public:

    const auto GetSelectedActor() const { return mSelectedActor; }

  private:

    void DrawActorRecursive(Actor* Actor);

  private:

    Actor* mSelectedActor = nullptr;
  };
}