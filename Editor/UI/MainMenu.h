#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class MainMenu : public Interface
  {
  public:

    virtual void Update() override;
    virtual void Draw() override;
  };
}