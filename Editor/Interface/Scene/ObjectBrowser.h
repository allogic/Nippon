#pragma once

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class ObjectBrowser : public Interface
  {
  public:

    virtual void Reset() override;
    virtual void Draw() override;
  };
}