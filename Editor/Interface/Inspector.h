#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Inspector : public Interface
  {
  public:

    virtual void Reset() override;
    virtual void Draw() override;
  };
}