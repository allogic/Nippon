#pragma once

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class ModelBrowser : public Interface
  {
  public:

    virtual void Reset() override;
    virtual void Draw() override;
  };
}