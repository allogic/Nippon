#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Component.h>
#include <Editor/Vertex.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  template<typename V, typename E>
  class Mesh;

  class Actor;

  class Renderable : public Component
  {
  public:

    Renderable(Actor* Actor);

  private:

    std::vector<Mesh<DefaultVertex, U32>*> mMeshes = {};
  };
}