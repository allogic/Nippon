#pragma once

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Interface
  {
  public:

    virtual ~Interface() {};

  public:

    virtual void Reset() = 0;
    virtual void Draw() = 0;
  };
}