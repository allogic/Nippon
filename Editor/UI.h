#pragma once

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class UI
  {
  public:

    UI();
    virtual ~UI();

  public:

    virtual void Update() = 0;
    virtual void Draw() = 0;
  };
}