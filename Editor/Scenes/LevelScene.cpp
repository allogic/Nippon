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
		fs::path relativeDatFileDir = fs::path{ GetGroupKey() } / GetDatArchiveFileName();
		fs::path relativeBinFileDir = fs::path{ GetGroupKey() } / GetBinArchiveFileName();

		fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;
		fs::path absoluteBinFileDir = gDataDir / relativeBinFileDir;

		std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

		mCipher->Decrypt(datBytes);

		mDatArchive = new Archive{ datBytes };

		mDatArchive->Load();

		if (fs::exists(absoluteBinFileDir))
		{
			std::vector<U8> binBytes = FsUtils::ReadBinary(absoluteBinFileDir);

			mCipher->Decrypt(binBytes);

			mBinArchive = new Archive{ binBytes };
		
			mBinArchive->Load();
		}
	}

	void LevelScene::LoadLevel()
	{
		mDatArchive->FindNodeRecursiveByType("TSC", &mTscNode);
		mDatArchive->FindNodeRecursiveByType("TRE", &mTreNode);
		mDatArchive->FindNodeRecursiveByType("TAT", &mTatNode);

		mDatArchive->FindNodesRecursiveByType("SCR", mScrNodes);
		mDatArchive->FindNodesRecursiveByType("DDS", mDdsNodes);

		const auto& tscObjects = ObjSerializer::FromBytes(mTscNode->GetBytes());
		const auto& treObjects = ObjSerializer::FromBytes(mTreNode->GetBytes());
		const auto& tatObjects = ObjSerializer::FromBytes(mTatNode->GetBytes());

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

		for (const auto& node : mScrNodes)
		{
			ScrGroup& group = mScrGroups.emplace_back(ScrSerializer::FromBytes(node->GetBytes()));

			group.Name = node->GetName();
		}

		for (const auto& node : mDdsNodes)
		{
			mScrTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytes()));
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
					fs::path relativeDatFileDir = fs::path{ entityData.SceneInfo->GroupKey } / entityData.SceneInfo->DatArchiveFileName;

					fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;

					std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

					mCipher->Decrypt(datBytes);

					entity.Archive->SetBytes(datBytes);
					entity.Archive->Load();

					entity.Archive->FindNodesRecursiveByType("MD", entityData.MdNodes);
					entity.Archive->FindNodesRecursiveByType("DDS", entityData.DdsNodes);

					for (const auto& node : entityData.MdNodes)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(node->GetBytes()));

						mdGroup.Name = node->GetName();
					}

					for (const auto& node : entityData.DdsNodes)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytes()));
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
					fs::path relativeDatFileDir = fs::path{ entityData.SceneInfo->GroupKey } / entityData.SceneInfo->DatArchiveFileName;

					fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;

					std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

					mCipher->Decrypt(datBytes);

					entity.Archive->SetBytes(datBytes);
					entity.Archive->Load();

					entity.Archive->FindNodesRecursiveByType("MD", entityData.MdNodes);
					entity.Archive->FindNodesRecursiveByType("DDS", entityData.DdsNodes);

					for (const auto& node : entityData.MdNodes)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(node->GetBytes()));

						mdGroup.Name = node->GetName();
					}

					for (const auto& node : entityData.DdsNodes)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytes()));
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
					fs::path relativeDatFileDir = fs::path{ entityData.SceneInfo->GroupKey } / entityData.SceneInfo->DatArchiveFileName;

					fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;

					std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

					mCipher->Decrypt(datBytes);

					entity.Archive->SetBytes(datBytes);
					entity.Archive->Load();

					entity.Archive->FindNodesRecursiveByType("MD", entityData.MdNodes);
					entity.Archive->FindNodesRecursiveByType("DDS", entityData.DdsNodes);

					for (const auto& node : entityData.MdNodes)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(node->GetBytes()));

						mdGroup.Name = node->GetName();
					}

					for (const auto& node : entityData.DdsNodes)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytes()));
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
		LOG("=============================================================\n");
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
		LOG("        Found %u TSC files\n", mTscNode ? 1 : 0);
		LOG("        Found %u TRE files\n", mTreNode ? 1 : 0);
		LOG("        Found %u TAT files\n", mTatNode ? 1 : 0);
		LOG("        Found %u SCR files\n", (U32)mScrNodes.size());
		LOG("        Found %u DDS files\n", (U32)mDdsNodes.size());

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
		for (const auto& node : mScrNodes)
		{
			LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
		}

		for (const auto& [archiveFileName, archive] : mTscArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& node : mTscEntityDataCache[archive].MdNodes)
			{
				LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, archive] : mTreArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& node : mTreEntityDataCache[archive].MdNodes)
			{
				LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, archive] : mTatArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& node : mTatEntityDataCache[archive].MdNodes)
			{
				LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
			}
		}

		LOG("\n");
		LOG("Loading textures:\n");

		LOG("    %s\n", GetDatArchiveFileName().c_str());
		for (const auto& node : mDdsNodes)
		{
			LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
		}

		for (const auto& [archiveFileName, archive] : mTscArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& node : mTscEntityDataCache[archive].DdsNodes)
			{
				LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, archive] : mTreArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& node : mTreEntityDataCache[archive].DdsNodes)
			{
				LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
			}
		}

		for (const auto& [archiveFileName, archive] : mTatArchiveCache)
		{
			LOG("    %s\n", archiveFileName.c_str());
			for (const auto& node : mTatEntityDataCache[archive].DdsNodes)
			{
				LOG("        %s.%s\n", node->GetName().c_str(), node->GetType().c_str());
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
