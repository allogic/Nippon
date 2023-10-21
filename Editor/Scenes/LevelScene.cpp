#include <Common/Macros.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

#include <Editor/Actor.h>
#include <Editor/Editor.h>
#include <Editor/TextureLoader.h>
#include <Editor/Magic.h>

#include <Editor/Scenes/LevelScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Serializer/ScrSerializer.h>
#include <Editor/Serializer/MdSerializer.h>
#include <Editor/Serializer/ObjSerializer.h>

namespace ark
{
	static std::map<U8, std::string> sCategoryToGroupKey =
	{
		//{ 0x09, "" }, // 0xFC 0x1E
		{ 0, "pl" },
		{ 0, "em" },
		{ 0, "an" },
		{ 0x03, "et" },
		{ 0x08, "ut" },
		{ 0, "es" },
		{ 0, "gt" },
		{ 0, "hm" },
		{ 0, "hl" },
		{ 0x0A, "it" },
		{ 0x0B, "vt" },
		{ 0x0C, "dr" },
		{ 0x0F, "us" },
		{ 0, "wp" },
	};

	LevelScene::LevelScene(const SceneInfo& Info) : Scene{ Info }
	{
		mCipher = new BlowFish{ gBlowFishKey };

		mLevelGeometryActor = CreateActor<Actor>("Level Geometry", nullptr);
		mEntityGeometryActor = CreateActor<Actor>("Entity Geometry", nullptr);
	}

	LevelScene::~LevelScene()
	{
		// TODO: Delete all entity textures..

		for (auto& texture : mScrTextures)
		{
			delete texture;
			texture = nullptr;
		}

		for (auto& [archiveName, archive] : mTatArchiveCache)
		{
			delete archive;
			archive = nullptr;
		}

		for (auto& [archiveName, archive] : mTreArchiveCache)
		{
			delete archive;
			archive = nullptr;
		}

		for (auto& [archiveName, archive] : mTscArchiveCache)
		{
			delete archive;
			archive = nullptr;
		}

		if (mBinArchive)
		{
			delete mBinArchive;
			mBinArchive = nullptr;
		}

		if (mDatArchive)
		{
			delete mDatArchive;
			mDatArchive = nullptr;
		}

		if (mCipher)
		{
			delete mCipher;
			mCipher = nullptr;
		}
	}

	void LevelScene::Load()
	{
		LoadArchives();
		LoadLevel();
		LoadEntities();

		AddStaticGeometry();

		if (mEnableConsole)
		{
			PrintSummary();
		}
	}

	void LevelScene::Save()
	{

	}

	void LevelScene::LoadArchives()
	{
		fs::path relativeDatFile = fs::path{ GetGroupKey() } / GetDatArchiveFileName();
		fs::path relativeBinFile = fs::path{ GetGroupKey() } / GetBinArchiveFileName();

		fs::path absoluteDatFile = gDataDir / relativeDatFile;
		fs::path absoluteBinFile = gDataDir / relativeBinFile;

		std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFile);

		mCipher->Decrypt(datBytes);

		mDatArchive = new Archive;

		mDatArchive->DeSerialize(datBytes);

		if (fs::exists(absoluteBinFile))
		{
			std::vector<U8> binBytes = FsUtils::ReadBinary(absoluteBinFile);

			mCipher->Decrypt(binBytes);

			mBinArchive = new Archive;
		
			mBinArchive->DeSerialize(binBytes);
		}
	}

	void LevelScene::LoadLevel()
	{
		mDatArchive->FindArchiveByType("TSC", &mTscArchive);
		mDatArchive->FindArchiveByType("TRE", &mTreArchive);
		mDatArchive->FindArchiveByType("TAT", &mTatArchive);

		mDatArchive->FindArchivesByType("SCR", mScrArchives);
		mDatArchive->FindArchivesByType("DDS", mDdsArchives);

		const auto& tscObjects = ObjSerializer::FromBytes(mTscArchive->GetBytes(), mTscArchive->GetSize());
		const auto& treObjects = ObjSerializer::FromBytes(mTreArchive->GetBytes(), mTreArchive->GetSize());
		const auto& tatObjects = ObjSerializer::FromBytes(mTatArchive->GetBytes(), mTatArchive->GetSize());

		for (U32 i = 0; i < (U32)tscObjects.size(); i++)
		{
			SceneInfo* sceneInfo = ObjectToSceneInfo(tscObjects[i]);

			if (sceneInfo)
			{
				if (!mTscArchiveCache[sceneInfo->DatArchiveFileName])
				{
					Archive* archive = new Archive;

					mTscArchiveCache[sceneInfo->DatArchiveFileName] = archive;
					mTscEntityDataCache[archive].SceneInfo = sceneInfo;
				}

				mTscEntities.emplace_back(i, tscObjects[i], mTscArchiveCache[sceneInfo->DatArchiveFileName]);
			}
		}

		for (U32 i = 0; i < (U32)treObjects.size(); i++)
		{
			SceneInfo* sceneInfo = ObjectToSceneInfo(treObjects[i]);

			if (sceneInfo)
			{
				if (!mTreArchiveCache[sceneInfo->DatArchiveFileName])
				{
					Archive* archive = new Archive;

					mTreArchiveCache[sceneInfo->DatArchiveFileName] = archive;
					mTreEntityDataCache[archive].SceneInfo = sceneInfo;
				}

				mTreEntities.emplace_back(i, treObjects[i], mTreArchiveCache[sceneInfo->DatArchiveFileName]);
			}
		}

		for (U32 i = 0; i < (U32)tatObjects.size(); i++)
		{
			SceneInfo* sceneInfo = ObjectToSceneInfo(tatObjects[i]);

			if (sceneInfo)
			{
				if (!mTatArchiveCache[sceneInfo->DatArchiveFileName])
				{
					Archive* archive = new Archive;

					mTatArchiveCache[sceneInfo->DatArchiveFileName] = archive;
					mTatEntityDataCache[archive].SceneInfo = sceneInfo;
				}

				mTatEntities.emplace_back(i, tatObjects[i], mTatArchiveCache[sceneInfo->DatArchiveFileName]);
			}
		}

		for (const auto& archive : mScrArchives)
		{
			ScrGroup& group = mScrGroups.emplace_back(ScrSerializer::FromBytes(archive->GetBytes(), archive->GetSize()));

			group.Name = archive->GetName();
		}

		for (const auto& archive : mDdsArchives)
		{
			mScrTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(archive->GetBytes(), archive->GetSize()));
		}
	}

	void LevelScene::LoadEntities()
	{
		for (auto& entity : mTscEntities)
		{
			auto& entityData = mTscEntityDataCache[entity.Archive];

			if (!entityData.Loaded)
			{
				entityData.Loaded = true;

				// TODO: Resolve missing category groups keys!
				if (entityData.SceneInfo->DatArchiveFileName != "")
				{
					fs::path relativeDatFile = fs::path{ entityData.SceneInfo->GroupKey } / entityData.SceneInfo->DatArchiveFileName;

					fs::path absoluteDatFile = gDataDir / relativeDatFile;

					std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFile);

					mCipher->Decrypt(datBytes);

					entity.Archive->DeSerialize(datBytes);

					entity.Archive->FindArchivesByType("MD", entityData.MdArchives);
					entity.Archive->FindArchivesByType("DDS", entityData.DdsArchives);

					for (const auto& archive : entityData.MdArchives)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(archive->GetBytes(), archive->GetSize()));

						mdGroup.Name = archive->GetName();
					}

					for (const auto& archive : entityData.DdsArchives)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(archive->GetBytes(), archive->GetSize()));
					}
				}
			}
		}

		for (auto& entity : mTreEntities)
		{
			auto& entityData = mTreEntityDataCache[entity.Archive];

			if (!entityData.Loaded)
			{
				entityData.Loaded = true;

				// TODO: Resolve missing category groups keys!
				if (entityData.SceneInfo->DatArchiveFileName != "")
				{
					fs::path relativeDatFile = fs::path{ entityData.SceneInfo->GroupKey } / entityData.SceneInfo->DatArchiveFileName;

					fs::path absoluteDatFile = gDataDir / relativeDatFile;

					std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFile);

					mCipher->Decrypt(datBytes);

					entity.Archive->DeSerialize(datBytes);

					entity.Archive->FindArchivesByType("MD", entityData.MdArchives);
					entity.Archive->FindArchivesByType("DDS", entityData.DdsArchives);

					for (const auto& archive : entityData.MdArchives)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(archive->GetBytes(), archive->GetSize()));

						mdGroup.Name = archive->GetName();
					}

					for (const auto& archive : entityData.DdsArchives)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(archive->GetBytes(), archive->GetSize()));
					}
				}
			}
		}

		for (auto& entity : mTatEntities)
		{
			auto& entityData = mTatEntityDataCache[entity.Archive];

			if (!entityData.Loaded)
			{
				entityData.Loaded = true;

				// TODO: Resolve missing category groups keys!
				if (entityData.SceneInfo->DatArchiveFileName != "")
				{
					fs::path relativeDatFile = fs::path{ entityData.SceneInfo->GroupKey } / entityData.SceneInfo->DatArchiveFileName;

					fs::path absoluteDatFile = gDataDir / relativeDatFile;

					std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFile);

					mCipher->Decrypt(datBytes);

					entity.Archive->DeSerialize(datBytes);

					entity.Archive->FindArchivesByType("MD", entityData.MdArchives);
					entity.Archive->FindArchivesByType("DDS", entityData.DdsArchives);

					for (const auto& archive : entityData.MdArchives)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(archive->GetBytes(), archive->GetSize()));

						mdGroup.Name = archive->GetName();
					}

					for (const auto& archive : entityData.DdsArchives)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(archive->GetBytes(), archive->GetSize()));
					}
				}
			}
		}
	}

	void LevelScene::AddStaticGeometry()
	{
		for (const auto& group : mScrGroups)
		{
			Actor* groupActor = CreateActor<Actor>(group.Name, mLevelGeometryActor);

			for (const auto& model : group.Models)
			{
				Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

				Transform* modelTransform = modelActor->GetTransform();

				const ScrTransform& transform = model.Transform;

				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } * MAGIC_WORLD_SCALE;
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * MAGIC_WORLD_SCALE;

				modelTransform->SetLocalPosition(position);

				for (const auto& division : model.Entry.Divisions)
				{
					Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

					Transform* divisionTransform = divisonActor->GetComponent<Transform>();

					divisionTransform->SetLocalRotation(rotation);
					divisionTransform->SetLocalScale(scale);

					Renderable* divisionRenderable = divisonActor->AttachComponent<Renderable>();

					std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(divisonActor, division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
					std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);

					U32 textureIndex = division.Header.TextureIndex;

					Texture2D* texture = (textureIndex < mScrTextures.size()) ? mScrTextures[textureIndex] : nullptr;

					divisionRenderable->SetVertexBuffer(vertices);
					divisionRenderable->SetElementBuffer(elements);
					divisionRenderable->LocalToRemote();
					divisionRenderable->SetTexture(textureIndex, texture);

					divisonActor->ComputeAABB();
				}
			}
		}

		for (const auto& entity : mTscEntities)
		{
			auto& entityData = mTscEntityDataCache[entity.Archive];

			for (const auto& group : entityData.MdGroups)
			{
				std::string groupName = group.Name + "_" + std::to_string(entity.Index);

				Actor* groupActor = CreateActor<Actor>(groupName, mEntityGeometryActor);

				for (const auto& model : group.Models)
				{
					Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

					Transform* modelTransform = modelActor->GetTransform();

					R32V3 position = R32V3{ entity.Object.Position.x, entity.Object.Position.y, entity.Object.Position.z } * MAGIC_WORLD_SCALE;
					R32V3 rotation = R32V3{ entity.Object.Rotation.x, entity.Object.Rotation.y, entity.Object.Rotation.z } + R32V3{ MAGIC_ENTITY_TO_LEVEL_PITCH_OFFSET, MAGIC_ENTITY_TO_LEVEL_YAW_OFFSET, MAGIC_ENTITY_TO_LEVEL_ROLL_OFFSET };
					R32V3 scale = R32V3{ entity.Object.Scale.x, entity.Object.Scale.y, entity.Object.Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;

					modelTransform->SetLocalPosition(position);

					for (const auto& division : model.Entry.Divisions)
					{
						Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

						Transform* divisionTransform = divisonActor->GetComponent<Transform>();

						divisionTransform->SetLocalRotation(rotation);
						divisionTransform->SetLocalScale(scale);

						Renderable* divisionRenderable = divisonActor->AttachComponent<Renderable>();

						std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(divisonActor, division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
						std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);

						U32 textureIndex = division.Header.TextureIndex;

						Texture2D* texture = (textureIndex < entityData.MdTextures.size()) ? entityData.MdTextures[textureIndex] : nullptr;

						divisionRenderable->SetVertexBuffer(vertices);
						divisionRenderable->SetElementBuffer(elements);
						divisionRenderable->LocalToRemote();
						divisionRenderable->SetTexture(textureIndex, texture);

						divisonActor->ComputeAABB();
					}
				}
			}
		}

		for (const auto& entity : mTreEntities)
		{
			auto& entityData = mTreEntityDataCache[entity.Archive];

			for (const auto& group : entityData.MdGroups)
			{
				std::string groupName = group.Name + "_" + std::to_string(entity.Index);

				Actor* groupActor = CreateActor<Actor>(groupName, mEntityGeometryActor);

				for (const auto& model : group.Models)
				{
					Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

					Transform* modelTransform = modelActor->GetTransform();

					R32V3 position = R32V3{ entity.Object.Position.x, entity.Object.Position.y, entity.Object.Position.z } * MAGIC_WORLD_SCALE;
					R32V3 rotation = R32V3{ entity.Object.Rotation.x, entity.Object.Rotation.y, entity.Object.Rotation.z } + R32V3{ MAGIC_ENTITY_TO_LEVEL_PITCH_OFFSET, MAGIC_ENTITY_TO_LEVEL_YAW_OFFSET, MAGIC_ENTITY_TO_LEVEL_ROLL_OFFSET };
					R32V3 scale = R32V3{ entity.Object.Scale.x, entity.Object.Scale.y, entity.Object.Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;

					modelTransform->SetLocalPosition(position);

					for (const auto& division : model.Entry.Divisions)
					{
						Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

						Transform* divisionTransform = divisonActor->GetComponent<Transform>();

						divisionTransform->SetLocalRotation(rotation);
						divisionTransform->SetLocalScale(scale);

						Renderable* divisionRenderable = divisonActor->AttachComponent<Renderable>();

						std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(divisonActor, division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
						std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);

						U32 textureIndex = division.Header.TextureIndex;

						Texture2D* texture = (textureIndex < entityData.MdTextures.size()) ? entityData.MdTextures[textureIndex] : nullptr;

						divisionRenderable->SetVertexBuffer(vertices);
						divisionRenderable->SetElementBuffer(elements);
						divisionRenderable->LocalToRemote();
						divisionRenderable->SetTexture(textureIndex, texture);

						divisonActor->ComputeAABB();
					}
				}
			}
		}

		for (const auto& entity : mTatEntities)
		{
			auto& entityData = mTatEntityDataCache[entity.Archive];

			for (const auto& group : entityData.MdGroups)
			{
				std::string groupName = group.Name + "_" + std::to_string(entity.Index);

				Actor* groupActor = CreateActor<Actor>(groupName, mEntityGeometryActor);

				for (const auto& model : group.Models)
				{
					Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

					Transform* modelTransform = modelActor->GetTransform();

					R32V3 position = R32V3{ entity.Object.Position.x, entity.Object.Position.y, entity.Object.Position.z } * MAGIC_WORLD_SCALE;
					R32V3 rotation = R32V3{ entity.Object.Rotation.x, entity.Object.Rotation.y, entity.Object.Rotation.z } + R32V3{ MAGIC_ENTITY_TO_LEVEL_PITCH_OFFSET, MAGIC_ENTITY_TO_LEVEL_YAW_OFFSET, MAGIC_ENTITY_TO_LEVEL_ROLL_OFFSET };
					R32V3 scale = R32V3{ entity.Object.Scale.x, entity.Object.Scale.y, entity.Object.Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;

					modelTransform->SetLocalPosition(position);

					for (const auto& division : model.Entry.Divisions)
					{
						Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

						Transform* divisionTransform = divisonActor->GetComponent<Transform>();

						divisionTransform->SetLocalRotation(rotation);
						divisionTransform->SetLocalScale(scale);

						Renderable* divisionRenderable = divisonActor->AttachComponent<Renderable>();

						std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(divisonActor, division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
						std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);

						U32 textureIndex = division.Header.TextureIndex;

						Texture2D* texture = (textureIndex < entityData.MdTextures.size()) ? entityData.MdTextures[textureIndex] : nullptr;

						divisionRenderable->SetVertexBuffer(vertices);
						divisionRenderable->SetElementBuffer(elements);
						divisionRenderable->LocalToRemote();
						divisionRenderable->SetTexture(textureIndex, texture);

						divisonActor->ComputeAABB();
					}
				}
			}
		}
	}

	SceneInfo* LevelScene::ObjectToSceneInfo(const ObjEntry& Object)
	{
		std::string groupKey = sCategoryToGroupKey[Object.Category];
		std::string sceneKey = StringUtils::ByteToString(Object.Id);

		// TODO: Resolve missing category groups keys!
		if (groupKey != "")
		{
			return SceneInfos::GetEntityByKey(groupKey, sceneKey);
		}

		return {};
	}

	void LevelScene::PrintSummary()
	{
		LOG("\n");
		LOG(" Opening Level Scene \\%s\\%s\n", GetGroupKey().c_str(), GetSceneKey().c_str());
		LOG("-----------------------------------------------------------------------------------------\n");
		LOG("\n");
		LOG("Loading archives:\n");
		LOG("    %s\n", GetDatArchiveFileName().c_str());
		LOG("    %s\n", GetBinArchiveFileName().c_str());

		for (const auto& [archiveFileName, archive] : mTscArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
		}

		for (const auto& [archiveFileName, archive] : mTreArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
		}

		for (const auto& [archiveFileName, archive] : mTatArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
		}

		LOG("\n");
		LOG("Searching files:\n");
		LOG("    %s\n", GetDatArchiveFileName().c_str());
		LOG("        Found %u TSC files\n", mTscArchive ? 1 : 0);
		LOG("        Found %u TRE files\n", mTreArchive ? 1 : 0);
		LOG("        Found %u TAT files\n", mTatArchive ? 1 : 0);
		LOG("        Found %u SCR files\n", (U32)mScrArchives.size());
		LOG("        Found %u DDS files\n", (U32)mDdsArchives.size());

		for (const auto& [archiveFileName, archive] : mTscArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			LOG("        Found %u MD files\n", (U32)mTscEntityDataCache[archive].MdGroups.size());
			LOG("        Found %u DDS files\n", (U32)mTscEntityDataCache[archive].MdTextures.size());
		}

		for (const auto& [archiveFileName, archive] : mTreArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			LOG("        Found %u MD files\n", (U32)mTreEntityDataCache[archive].MdGroups.size());
			LOG("        Found %u DDS files\n", (U32)mTreEntityDataCache[archive].MdTextures.size());
		}

		for (const auto& [archiveFileName, archive] : mTatArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			LOG("        Found %u MD files\n", (U32)mTatEntityDataCache[archive].MdGroups.size());
			LOG("        Found %u DDS files\n", (U32)mTatEntityDataCache[archive].MdTextures.size());
		}

		LOG("\n");
		LOG("Loading models:\n");

		LOG("    %s\n", GetDatArchiveFileName().c_str());
		for (const auto& archive : mScrArchives)
		{
			LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
		}

		for (const auto& [archiveFileName, cachedArchive] : mTscArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& archive : mTscEntityDataCache[cachedArchive].MdArchives)
			{
				LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, cachedArchive] : mTreArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& archive : mTreEntityDataCache[cachedArchive].MdArchives)
			{
				LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, cachedArchive] : mTatArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& archive : mTatEntityDataCache[cachedArchive].MdArchives)
			{
				LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
			}
		}

		LOG("\n");
		LOG("Loading textures:\n");

		LOG("    %s\n", GetDatArchiveFileName().c_str());
		for (const auto& archive : mDdsArchives)
		{
			LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
		}

		for (const auto& [archiveFileName, cachedArchive] : mTscArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& archive : mTscEntityDataCache[cachedArchive].DdsArchives)
			{
				LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, cachedArchive] : mTreArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& archive : mTreEntityDataCache[cachedArchive].DdsArchives)
			{
				LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, cachedArchive] : mTatArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& archive : mTatEntityDataCache[cachedArchive].DdsArchives)
			{
				LOG("        %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
			}
		}

		LOG("\n");
		LOG("Loading TSC objects:\n");

		for (const auto& entity : mTscEntities)
		{
			LOG("    Id=0x%02X Category=0x%02X\n", entity.Object.Id, entity.Object.Category);
		}

		LOG("\n");
		LOG("Loading TRE objects:\n");
		for (const auto& entity : mTreEntities)
		{
			LOG("    Id=0x%02X Category=0x%02X\n", entity.Object.Id, entity.Object.Category);
		}

		LOG("\n");
		LOG("Loading TAT objects:\n");
		for (const auto& entity : mTatEntities)
		{
			LOG("    Id=0x%02X Category=0x%02X\n", entity.Object.Id, entity.Object.Category);
		}

		LOG("\n");
	}
}
