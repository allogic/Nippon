#pragma once

#include <string>

#include <Common/Types.h>

#include <Editor/Actor.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Box : public Actor
  {
  public:

    Box(std::string const& Name);

  public:

    void Update(ark::R32 TimeDelta) override;
  };
}