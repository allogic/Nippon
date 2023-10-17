#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <Common/Types.h>

#include <Editor/Generated/SceneInfos.h>

namespace ark
{
	class Scene;

	class SceneManager
	{
	public:

		static void Create();
		static void Destroy();

	public:

		static Scene* GetActiveScene();

	public:

		static void SetDirty(bool Value);
		static void SetActiveScene(Scene* Scene);

	public:

		static void PreRender();
		static void Render();
		static void PostRender();

		static void PreUpdate();
		static void Update();
		static void PostUpdate();

	public:

		static Scene* CreateScene(const SceneInfo& Info);
		static void DestroyScene(Scene* Scene);
	};
}
