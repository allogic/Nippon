#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>
#include <Macros.h>

#include <Database/Database.h>

namespace Nippon
{
	class SceneManager
	{
	public:

		static void Create();
		static void Destroy();

	public:

		static Scene* GetCurrentScene();

	public:

		static void SetDirty();
		static void SetCurrentScene(Scene* Scene);

	public:

		static void FixedUpdate();
		static void Update();
		static void Render();

	private:

		static void CheckScenesToBeDestroyed();

	public:

		static Scene* CreateScene(ArchiveInfo const& ArchiveInfo, bool SkipSceneEmplacement = false);
		static void DestroyScene(Scene* Scene, bool DestroyImmediately = false);
	};
}