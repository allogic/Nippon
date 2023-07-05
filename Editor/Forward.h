#pragma once

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

struct GLFWwindow;

namespace ark
{
	class Box;
	class Player;

	class Camera;
	class Renderable;
	class Transform;

	class ModelSerializer;
	class ObjectSerializer;

	class DebugRenderer;
	class DefaultRenderer;

	class EntityScene;
	class LevelScene;

	class MainMenu;
	class Inspector;
	class LevelBrowser;
	class EntityBrowser;
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

	class Scene;
	class SceneManager;
	class InterfaceManager;
	class Math;
	class Packer;
	class Shader;
	class Texture2D;
	class RenderTexture;
	class DepthStencilTexture;

	struct DefaultVertex;
	struct DebugVertex;
}