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

    virtual void Draw() override;

  private:

    bool HasResized();

  private:

    U32 mWidth = 0;
    U32 mHeight = 0;
  };
}