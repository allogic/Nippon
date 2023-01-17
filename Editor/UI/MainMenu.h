#pragma once

#include <Common/Types.h>

#include <Editor/UI.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class MainMenu : public UI
  {
  public:

    virtual void Update() override;
    virtual void Draw() override;
  };
}