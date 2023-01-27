#pragma once

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Interface
  {
  public:

    virtual ~Interface() = default;

  public:

    virtual void Update() = 0;
    virtual void Draw() = 0;
  };
}