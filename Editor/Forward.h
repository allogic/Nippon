#pragma once

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

struct GLFWwindow;

namespace ark
{
  class Player;

  class Camera;
  class Renderable;
  class Transform;

  class ModelSerializer;
  class ObjectSerializer;

  class DebugRenderer;
  class DefaultRenderer;

  class MainMenu;
  class ModelBrowser;
  class ObjectBrowser;
  class Outline;

  class Actor;
  class Archive;
  class Component;
  class Event;
  class FrameBuffer;
  class Interface;

  template<typename V, typename E>
  class Mesh;

  class Packer;
  class Scene;
  class Shader;

  template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal, typename T>
  class Texture;

  struct DefaultVertex;
  struct DebugVertex;
}