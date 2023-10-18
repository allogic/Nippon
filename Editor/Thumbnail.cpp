#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/TextureUtils.h>

#include <Editor/Actor.h>
#include <Editor/Thumbnail.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Camera.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	void Thumbnail::Generate(const SceneInfo& Info)
	{
		fs::path exportDir = "Thumbnails";

		FsUtils::CreateIfNotExist(exportDir);

		if (Scene* scene = SceneManager::CreateScene(Info))
		{
			Actor* player = scene->GetPlayerActor();
			Camera* camera = scene->GetMainCamera();

			player->GetTransform()->SetLocalPosition(R32V3{ 100.0F, 50.0F, 100.0F });
			player->GetTransform()->SetLocalRotation(R32V3{ -20.0F, 45.0F, 0.0F });

			scene->Load();
			scene->Resize(128, 128);
			scene->Step();

			std::vector<U8> bytes = scene->Snapshot(4, GL_UNSIGNED_BYTE);

			TextureUtils::WritePNG(128, 128, bytes, exportDir / scene->GetThumbnailFileName());

			SceneManager::DestroyScene(scene);
		}
	}
}
