#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Component.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Camera : public Component
  {
  public:

    Camera(Actor* Actor);

  public:

    inline auto GetFov() const { return mFov; }
    inline auto GetNear() const { return mNear; }
    inline auto GetFar() const { return mFar; }

  public:

    inline void SetFov(R32 Fov) { mFov = Fov; }
    inline void SetNear(R32 Near) { mNear = Near; }
    inline void SetFar(R32 Far) { mFar = Far; }

  public:

    R32M4 GetProjectionMatrix();
    R32M4 GetViewMatrix();

  private:

    R32 mFov = 45.0F;
    R32 mNear = 0.1F;
    R32 mFar = 50000.0F;
  };
}