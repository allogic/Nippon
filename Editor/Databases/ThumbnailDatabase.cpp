#include <Common/Macros.h>
#include <Common/BinaryReader.h>

#include <Common/Utilities/FsUtils.h>

#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/FrameBuffer.h>
#include <Editor/Texture2D.h>

#include <Editor/Components/Transform.h>

#include <Editor/Databases/FileDatabase.h>
#include <Editor/Databases/ThumbnailDatabase.h>

#include <Editor/Utilities/ImageUtils.h>

namespace ark
{
	static std::map<U32, ThumbnailContainer> sThumbnailsByIdentifier = {};

	void ThumbnailDatabase::Create()
	{
		LoadThumbnails();
	}

	void ThumbnailDatabase::Generate()
	{
		fs::path databaseFile = "ThumbnailDatabase.db";

		std::ofstream stream = std::ofstream{ databaseFile, std::ios::binary };

		if (stream.is_open())
		{
			U32 entryCount = FileDatabase::GetTotalLevelCount() + FileDatabase::GetTotalEntityCount();

			stream.write((I8*)&entryCount, sizeof(U32));

			for (const auto& directory : FileDatabase::GetLevelDirectories())
			{
				for (const auto& fileContainer : FileDatabase::GetLevelFileContainersByDirectory(directory))
				{
					if (Scene* scene = SceneManager::CreateScene(&fileContainer))
					{
						LOG("Generating for \\%s\\%s\n", fileContainer.GetDirectoryId(), fileContainer.GetFileId());

						Actor* player = scene->GetPlayerActor();
						Camera* camera = scene->GetMainCamera();

						player->GetTransform()->SetLocalPosition(R32V3{ 100.0F, 50.0F, 100.0F });
						player->GetTransform()->SetLocalRotation(R32V3{ -20.0F, 45.0F, 0.0F });

						scene->Load();
						scene->Resize(128, 128);
						scene->Invalidate();

						U32 frameBuffer = scene->GetFrameBuffer();
						U32 colorTexture = FrameBuffer::GetColorTexture(frameBuffer, 0);

						U32 dummyTexture = 0;

						std::vector<U8> bytes = ImageUtils::WritePNG(colorTexture);
						U32 size = (U32)bytes.size();

						stream.write((I8*)&fileContainer.GetIdentifier(), sizeof(U32));
						stream.write((I8*)&fileContainer.GetType(), sizeof(U32));
						stream.write((I8*)&dummyTexture, sizeof(U32));
						stream.write((I8*)&size, sizeof(U32));
						stream.write((I8*)bytes.data(), bytes.size());

						SceneManager::DestroyScene(scene);
					}
				}
			}

			for (const auto& directory : FileDatabase::GetEntityDirectories())
			{
				for (const auto& fileContainer : FileDatabase::GetEntityFileContainersByDirectory(directory))
				{
					if (Scene* scene = SceneManager::CreateScene(&fileContainer))
					{
						LOG("Generating for \\%s\\%s\n", fileContainer.GetDirectoryId(), fileContainer.GetFileId());

						Actor* player = scene->GetPlayerActor();
						Camera* camera = scene->GetMainCamera();

						player->GetTransform()->SetLocalPosition(R32V3{ 100.0F, 50.0F, 100.0F });
						player->GetTransform()->SetLocalRotation(R32V3{ -20.0F, 45.0F, 0.0F });

						scene->Load();
						scene->Resize(128, 128);
						scene->Invalidate();

						U32 frameBuffer = scene->GetFrameBuffer();
						U32 colorTexture = FrameBuffer::GetColorTexture(frameBuffer, 0);

						U32 dummyTexture = 0;

						std::vector<U8> bytes = ImageUtils::WritePNG(colorTexture);
						U32 size = (U32)bytes.size();

						stream.write((I8*)&fileContainer.GetIdentifier(), sizeof(U32));
						stream.write((I8*)&fileContainer.GetType(), sizeof(U32));
						stream.write((I8*)&dummyTexture, sizeof(U32));
						stream.write((I8*)&size, sizeof(U32));
						stream.write((I8*)bytes.data(), bytes.size());

						SceneManager::DestroyScene(scene);
					}
				}
			}

			stream.close();
		}
	}

	void ThumbnailDatabase::Destroy()
	{
		for (const auto& [identifier, thumbnailContainer] : sThumbnailsByIdentifier)
		{
			Texture2D::Destroy(thumbnailContainer.mTexture);
		}
	}

	const ThumbnailContainer& ThumbnailDatabase::GetThumbnailContainerByIdentifier(U32 Identifier)
	{
		return sThumbnailsByIdentifier[Identifier];
	}

	void ThumbnailDatabase::LoadThumbnails()
	{
		fs::path databaseFile = "ThumbnailDatabase.db";

		if (fs::exists(databaseFile))
		{
			std::vector<U8> bytes = FsUtils::ReadBinary(databaseFile);

			BinaryReader reader = { bytes.data(), bytes.size() };

			U32 entryCount = reader.Read<U32>();

			for (U32 i = 0; i < entryCount; i++)
			{
				ThumbnailContainer thumbnailContainerTmp = reader.Read<ThumbnailContainer>();
				U32 thumbnailSize = reader.Read<U32>();
				std::vector<U8> bytes = reader.Bytes(thumbnailSize);

				sThumbnailsByIdentifier[thumbnailContainerTmp.mIdentifier] = thumbnailContainerTmp;

				ThumbnailContainer& thumbnailContainer = sThumbnailsByIdentifier[thumbnailContainerTmp.mIdentifier];

				thumbnailContainer.mTexture = ImageUtils::ReadPNG(bytes.data(), bytes.size());
			}
		}
		else
		{
			LOG("Thumbnail database %s does not exist\n", databaseFile.string().c_str());
		}
	}
}
