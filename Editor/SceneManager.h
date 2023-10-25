#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <Common/Types.h>

namespace ark
{
	class Scene;
	class FileContainer;

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

		static void Update();
		static void Render();

	public:

		static Scene* CreateScene(const FileContainer* FileContainer);
		static void DestroyScene(Scene* Scene);
	};
}
