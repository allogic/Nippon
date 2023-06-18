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
  class Inspector;
  class ModelBrowser;
  class ObjectBrowser;
  class TextureBrowser;
  class Outline;
  class Viewport;

  class Actor;
  class Archive;
  class Component;
  class Event;
  class FrameBuffer;
  class Integrity;
  class Interface;

  template<typename V, typename E>
  class Mesh;

  class Packer;
  class Scene;
  class Shader;
  class Texture2D;
  class RenderTexture;
  class DepthStencilTexture;

  struct DefaultVertex;
  struct DebugVertex;
}