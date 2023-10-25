#include <Common/Macros.h>
#include <Common/BinaryMediator.h>

#include <Common/Utilities/FsUtils.h>

#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/FrameBuffer.h>
#include <Editor/Texture2D.h>

#include <Editor/Components/Transform.h>

#include <Editor/Databases/FileDatabase.h>
#include <Editor/Databases/ThumbnailDatabase.h>

#include <Editor/Utilities/ImageUtils.h>

#define FILE_DATABASE_NAME "ThumbnailDatabase.db"

namespace ark
{
	static std::map<U32, const ThumbnailContainer*> sThumbnailsByIdentifier = {};

	void ThumbnailDatabase::Create()
	{
		LoadThumbnails();
	}

	void ThumbnailDatabase::Generate()
	{
		std::vector<U8> bytes = {};

		bytes.resize(12 * 1024 * 1024);

		BinaryMediator mediator = { bytes.data(), bytes .size() };

		U32 entryCount = FileDatabase::GetTotalLevelCount() + FileDatabase::GetTotalEntityCount();

		mediator.Write<U32>(entryCount);

		for (const auto& directory : FileDatabase::GetAllDirectories())
		{
			for (const auto& fileContainer : FileDatabase::GetFileContainersByDirectory(directory))
			{
				if (Scene* scene = SceneManager::CreateScene(fileContainer))
				{
					LOG("Generating for \\%s\\%s\n", fileContainer->GetDirectoryId(), fileContainer->GetFileId());

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

					mediator.Write<U32>(fileContainer->GetIdentifier());
					mediator.Write<U32>(fileContainer->GetType());
					mediator.Write<U32>(dummyTexture);
					mediator.Write<U32>((U32)bytes.size());
					mediator.WriteByteRange(bytes.data(), bytes.size());

					SceneManager::DestroyScene(scene);
				}
			}
		}

		FsUtils::WriteBinary(FILE_DATABASE_NAME, bytes);
	}

	void ThumbnailDatabase::Destroy()
	{
		for (const auto& [identifier, thumbnailContainer] : sThumbnailsByIdentifier)
		{
			if (thumbnailContainer->mTexture)
			{
				Texture2D::Destroy(thumbnailContainer->mTexture);
			}

			delete thumbnailContainer;
		}
	}

	const ThumbnailContainer* ThumbnailDatabase::GetThumbnailContainerByIdentifier(U32 Identifier)
	{
		return sThumbnailsByIdentifier[Identifier];
	}

	void ThumbnailDatabase::LoadThumbnails()
	{
		if (fs::exists(FILE_DATABASE_NAME))
		{
			std::vector<U8> bytes = FsUtils::ReadBinary(FILE_DATABASE_NAME);

			BinaryMediator mediator = { bytes.data(), bytes.size() };

			U32 entryCount = mediator.Read<U32>();

			for (U32 i = 0; i < entryCount; i++)
			{
				ThumbnailContainer* thumbnailContainer = new ThumbnailContainer;

				ThumbnailContainer dummyThumbnailContainer = mediator.Read<ThumbnailContainer>();

				std::memcpy(thumbnailContainer, &dummyThumbnailContainer, sizeof(ThumbnailContainer));

				sThumbnailsByIdentifier[thumbnailContainer->mIdentifier] = thumbnailContainer;

				U32 thumbnailSize = mediator.Read<U32>();
				std::vector<U8> bytes = mediator.ReadByteRange(thumbnailSize);

				thumbnailContainer->mTexture = ImageUtils::ReadPNG(bytes.data(), bytes.size());
			}
		}
		else
		{
			LOG("Thumbnail database does not exist\n");
		}
	}
}
