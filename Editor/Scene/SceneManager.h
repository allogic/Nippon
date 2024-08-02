#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>
#include <Common/Macros.h>

#include <Editor/Forward.h>

#include <Editor/Database/Database.h>

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

		static Scene* CreateSceneFromFile(fs::path const& FilePath, ArchiveType ArchiveType, bool SkipSceneEmplacement = false);
		static Scene* CreateSceneFromDatabase(ArchiveInfo const& ArchiveInfo, bool SkipSceneEmplacement = false);

		static void DestroyScene(Scene* Scene, bool DestroyImmediately = false);
	};
}