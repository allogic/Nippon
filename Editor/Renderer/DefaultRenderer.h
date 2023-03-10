#pragma once

#include <queue>
#include <filesystem>

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  struct RenderTask
  {
    const Transform* TransformPtr;
    const Mesh<DefaultVertex, U32>* MeshPtr;
  };

  class DefaultRenderer
  {
  public:

    DefaultRenderer();
    virtual ~DefaultRenderer();

  public:

    void Render();

  public:

    void AddRenderTask(const RenderTask& RenderTask);

  private:

    Shader* mShader;

    std::queue<RenderTask> mRenderQueue = {};
  };
}