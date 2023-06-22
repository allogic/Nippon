#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Viewport : public Interface
  {
  public:

    Viewport(Scene* Scene);

  public:

    inline auto IsFocused() const { return mIsFocused; }

  public:

    virtual void Reset() override;
    virtual void Draw() override;

  private:

    bool HasResized();

  private:

    Scene* mScene;

    U32 mWidth = 0;
    U32 mHeight = 0;

    bool mIsFocused = false;
  };
}