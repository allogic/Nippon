#pragma once

#include <algorithm>

#include <Common/Types.h>

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class TextureBrowser : public Interface
  {
  public:

    virtual void Reset() override;
    virtual void Draw() override;
  };
}