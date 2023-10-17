#include <Common/Macros.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

#include <Editor/Actor.h>
#include <Editor/Editor.h>
#include <Editor/TextureLoader.h>
#include <Editor/Coefficients.h>

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
	static std::map<U32, std::string> sCategoryToGroupKey =
	{
		//{ 0x09, "" }, // 0xFC 0x1E

		{ 0, "pl" },
		{ 0, "em" },
		{ 0, "an" },
		{ 0x03, "et" },
		{ 0x08, "ut" },
		{ 0, "es" },
		{ 0, "gt" },
		{ 0x0A, "it" },
		{ 0x0B, "vt" },
		{ 0x0C, "dr" },
		{ 0x0F, "us" },
		{ 0, "wp" },
	};

	LevelScene::LevelScene(const SceneInfo& Info) : Scene{ Info }
	{
		mObjectPlacementsActor = CreateActor<Actor>("Object Placements", nullptr);
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

		if (mArchive)
		{
			delete mArchive;
			mArchive = nullptr;
		}
	}

	void LevelScene::Load()
	{
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

	void LevelScene::LoadLevel()
	{
		fs::path relativeFilePath = fs::path{ GetGroupKey() } / GetArchiveFileName();
		fs::path absoluteFilePath = gDataDir / relativeFilePath;

		std::vector<U8> bytes = FsUtils::ReadBinary(absoluteFilePath);

		BlowFish{ gBlowFishKey }.Decrypt(bytes);

		mArchive = new Archive{ nullptr };

		mArchive->LoadRecursive(bytes, 0, 0, 0, "", GetArchiveFileName(), true);

		mArchive->FindNodeRecursiveByType("TSC", &mTscNode);
		mArchive->FindNodeRecursiveByType("TRE", &mTreNode);
		mArchive->FindNodeRecursiveByType("TAT", &mTatNode);

		mArchive->FindNodesRecursiveByType("SCR", mScrNodes);
		mArchive->FindNodesRecursiveByType("DDS", mDdsNodes);

		const auto& tscObjects = ObjSerializer::FromBytes(mTscNode->GetBytesWithoutHeader());
		const auto& treObjects = ObjSerializer::FromBytes(mTreNode->GetBytesWithoutHeader());
		const auto& tatObjects = ObjSerializer::FromBytes(mTatNode->GetBytesWithoutHeader());

		for (const auto& object : tscObjects)
		{
			const auto& sceneInfo = ObjectToSceneInfo(object);

			if (!mTscArchiveCache[sceneInfo.ArchiveFileName])
			{
				Archive* archive = new Archive{ nullptr };

				mTscArchiveCache[sceneInfo.ArchiveFileName] = archive;
				mTscEntityDataCache[archive].SceneInfo = sceneInfo;
			}

			mTscEntities.emplace_back(object, mTscArchiveCache[sceneInfo.ArchiveFileName]);
		}

		for (const auto& object : treObjects)
		{
			const auto& sceneInfo = ObjectToSceneInfo(object);

			if (!mTreArchiveCache[sceneInfo.ArchiveFileName])
			{
				Archive* archive = new Archive{ nullptr };

				mTreArchiveCache[sceneInfo.ArchiveFileName] = archive;
				mTreEntityDataCache[archive].SceneInfo = sceneInfo;
			}

			mTreEntities.emplace_back(object, mTreArchiveCache[sceneInfo.ArchiveFileName]);
		}

		for (const auto& object : tatObjects)
		{
			const auto& sceneInfo = ObjectToSceneInfo(object);

			if (!mTatArchiveCache[sceneInfo.ArchiveFileName])
			{
				Archive* archive = new Archive{ nullptr };

				mTatArchiveCache[sceneInfo.ArchiveFileName] = archive;
				mTatEntityDataCache[archive].SceneInfo = sceneInfo;
			}

			mTatEntities.emplace_back(object, mTatArchiveCache[sceneInfo.ArchiveFileName]);
		}

		for (const auto& node : mScrNodes)
		{
			ScrGroup& group = mScrGroups.emplace_back(ScrSerializer::FromBytes(node->GetBytesWithoutHeader()));

			group.Name = node->GetName();
		}

		for (const auto& node : mDdsNodes)
		{
			mScrTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytesWithoutHeader()));
		}
	}

	void LevelScene::LoadEntities()
	{
		BlowFish cipher = gBlowFishKey;

		for (auto& entity : mTscEntities)
		{
			auto& entityData = mTscEntityDataCache[entity.Archive];

			if (!entityData.Loaded)
			{
				entityData.Loaded = true;

				// TODO: Resolve missing category groups keys!
				if (entityData.SceneInfo.ArchiveFileName != "")
				{
					fs::path relativeFilePath = fs::path{ entityData.SceneInfo.GroupKey } / entityData.SceneInfo.ArchiveFileName;
					fs::path absoluteFilePath = gDataDir / relativeFilePath;

					std::vector<U8> bytes = FsUtils::ReadBinary(absoluteFilePath);

					cipher.Decrypt(bytes);

					entity.Archive->LoadRecursive(bytes, 0, 0, 0, "", entityData.SceneInfo.ArchiveFileName, true);

					entity.Archive->FindNodesRecursiveByType("MD", entityData.MdNodes);
					entity.Archive->FindNodesRecursiveByType("DDS", entityData.DdsNodes);

					for (const auto& node : entityData.MdNodes)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(node->GetBytesWithoutHeader()));

						mdGroup.Name = node->GetName();
					}

					for (const auto& node : entityData.DdsNodes)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytesWithoutHeader()));
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
				if (entityData.SceneInfo.ArchiveFileName != "")
				{
					fs::path relativeFilePath = fs::path{ entityData.SceneInfo.GroupKey } / entityData.SceneInfo.ArchiveFileName;
					fs::path absoluteFilePath = gDataDir / relativeFilePath;

					std::vector<U8> bytes = FsUtils::ReadBinary(absoluteFilePath);

					cipher.Decrypt(bytes);

					entity.Archive->LoadRecursive(bytes, 0, 0, 0, "", entityData.SceneInfo.ArchiveFileName, true);

					entity.Archive->FindNodesRecursiveByType("MD", entityData.MdNodes);
					entity.Archive->FindNodesRecursiveByType("DDS", entityData.DdsNodes);

					for (const auto& node : entityData.MdNodes)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(node->GetBytesWithoutHeader()));

						mdGroup.Name = node->GetName();
					}

					for (const auto& node : entityData.DdsNodes)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytesWithoutHeader()));
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
				if (entityData.SceneInfo.ArchiveFileName != "")
				{
					fs::path relativeFilePath = fs::path{ entityData.SceneInfo.GroupKey } / entityData.SceneInfo.ArchiveFileName;
					fs::path absoluteFilePath = gDataDir / relativeFilePath;

					std::vector<U8> bytes = FsUtils::ReadBinary(absoluteFilePath);

					cipher.Decrypt(bytes);

					entity.Archive->LoadRecursive(bytes, 0, 0, 0, "", entityData.SceneInfo.ArchiveFileName, true);

					entity.Archive->FindNodesRecursiveByType("MD", entityData.MdNodes);
					entity.Archive->FindNodesRecursiveByType("DDS", entityData.DdsNodes);

					for (const auto& node : entityData.MdNodes)
					{
						auto& mdGroup = entityData.MdGroups.emplace_back(MdSerializer::FromBytes(node->GetBytesWithoutHeader()));

						mdGroup.Name = node->GetName();
					}

					for (const auto& node : entityData.DdsNodes)
					{
						entityData.MdTextures.emplace_back(TextureLoader::LoadDirectDrawSurface(node->GetBytesWithoutHeader()));
					}
				}
			}
		}
	}

	void LevelScene::AddStaticGeometry()
	{
		for (const auto& group : mScrGroups)
		{
			Actor* groupActor = CreateActor<Actor>(group.Name, GetStaticGeometryActor());

			for (const auto& model : group.Models)
			{
				Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

				Transform* modelTransform = modelActor->GetTransform();

				const ScrTransform& transform = model.Transform;

				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } * DEBUG_WORLD_SCALE;
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * DEBUG_WORLD_SCALE;

				//modelTransform->SetLocalPosition(position);
				//modelTransform->SetLocalRotation(rotation);
				//modelTransform->SetLocalScale(scale);

				for (const auto& division : model.Entry.Divisions)
				{
					Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

					Transform* divisionTransform = divisonActor->GetComponent<Transform>();

					divisionTransform->SetLocalPosition(position);
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
				Actor* groupActor = CreateActor<Actor>(group.Name, GetStaticGeometryActor());

				for (const auto& model : group.Models)
				{
					Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

					Transform* modelTransform = modelActor->GetTransform();

					const MdTransform& transform = model.Transform;

					// Mostly has identity transformation (0,0,0) (0,0,0) (1,1,1)
					R32V3 entityPosition = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
					R32V3 entityRotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
					R32V3 entityScale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };

					R32V3 levelPosition = R32V3{ entity.Object.Position.x, entity.Object.Position.y, entity.Object.Position.z } * DEBUG_WORLD_SCALE;
					R32V3 levelRotation = glm::degrees(R32V3{ entity.Object.Rotation.x, entity.Object.Rotation.y, entity.Object.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
					R32V3 levelScale = R32V3{ entity.Object.Scale.x, entity.Object.Scale.y, entity.Object.Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;

					R32V3 position = levelPosition;
					R32V3 rotation = levelRotation;
					R32V3 scale = levelScale;

					//modelTransform->SetLocalPosition(position);
					//modelTransform->SetLocalRotation(rotation);
					//modelTransform->SetLocalScale(scale);

					for (const auto& division : model.Entry.Divisions)
					{
						Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

						Transform* divisionTransform = divisonActor->GetComponent<Transform>();

						divisionTransform->SetLocalPosition(position);
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
				Actor* groupActor = CreateActor<Actor>(group.Name, GetStaticGeometryActor());

				for (const auto& model : group.Models)
				{
					Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

					Transform* modelTransform = modelActor->GetTransform();

					const MdTransform& transform = model.Transform;

					// Mostly has identity transformation (0,0,0) (0,0,0) (1,1,1)
					R32V3 entityPosition = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
					R32V3 entityRotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
					R32V3 entityScale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };

					R32V3 levelPosition = R32V3{ entity.Object.Position.x, entity.Object.Position.y, entity.Object.Position.z } * DEBUG_WORLD_SCALE;
					R32V3 levelRotation = glm::degrees(R32V3{ entity.Object.Rotation.x, entity.Object.Rotation.y, entity.Object.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
					R32V3 levelScale = R32V3{ entity.Object.Scale.x, entity.Object.Scale.y, entity.Object.Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;

					R32V3 position = levelPosition;
					R32V3 rotation = levelRotation;
					R32V3 scale = levelScale;

					//modelTransform->SetLocalPosition(position);
					//modelTransform->SetLocalRotation(rotation);
					//modelTransform->SetLocalScale(scale);

					for (const auto& division : model.Entry.Divisions)
					{
						Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

						Transform* divisionTransform = divisonActor->GetComponent<Transform>();

						divisionTransform->SetLocalPosition(position);
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
				Actor* groupActor = CreateActor<Actor>(group.Name, GetStaticGeometryActor());

				for (const auto& model : group.Models)
				{
					Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

					Transform* modelTransform = modelActor->GetTransform();

					const MdTransform& transform = model.Transform;

					// Mostly has identity transformation (0,0,0) (0,0,0) (1,1,1)
					R32V3 entityPosition = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
					R32V3 entityRotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
					R32V3 entityScale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };

					R32V3 levelPosition = R32V3{ entity.Object.Position.x, entity.Object.Position.y, entity.Object.Position.z } * DEBUG_WORLD_SCALE;
					R32V3 levelRotation = glm::degrees(R32V3{ entity.Object.Rotation.x, entity.Object.Rotation.y, entity.Object.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
					R32V3 levelScale = R32V3{ entity.Object.Scale.x, entity.Object.Scale.y, entity.Object.Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;

					R32V3 position = levelPosition;
					R32V3 rotation = levelRotation;
					R32V3 scale = levelScale;

					//modelTransform->SetLocalPosition(position);
					//modelTransform->SetLocalRotation(rotation);
					//modelTransform->SetLocalScale(scale);

					for (const auto& division : model.Entry.Divisions)
					{
						Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

						Transform* divisionTransform = divisonActor->GetComponent<Transform>();

						divisionTransform->SetLocalPosition(position);
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

	/*
	void LevelScene::AddObjectPlacements()
	{
		Actor* tscObjectGroupActor = CreateActor<Actor>("TSC Group", mObjectPlacementsActor);
		Actor* treObjectGroupActor = CreateActor<Actor>("TRE Group", mObjectPlacementsActor);
		Actor* tatObjectGroupActor = CreateActor<Actor>("TAT Group", mObjectPlacementsActor);

		for (const auto& object : mTscObjects)
		{
			std::string objectName = "Object_" + std::to_string(object.Id) + "_" + std::to_string(object.Category);

			Actor* objectGroupActor = CreateActor<Actor>(objectName, tscObjectGroupActor);

			Transform* objectGroupTransform = objectGroupActor->GetTransform();

			R32V3 position = R32V3{ object.Position.x, object.Position.y, object.Position.z } * DEBUG_WORLD_SCALE;
			R32V3 rotation = glm::degrees(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
			R32V3 scale = R32V3{ object.Scale.x, object.Scale.y, object.Scale.z } / MAGIC_SCALE_COEFFICIENT * DEBUG_WORLD_SCALE;

			//objectGroupTransform->SetLocalPosition(position);
			//objectGroupTransform->SetLocalRotation(rotation);
			//objectGroupTransform->SetLocalScale(scale);

			Actor* objectActor = CreateActor<Actor>("Object", objectGroupActor);

			Transform* objectTransform = objectActor->GetTransform();

			objectTransform->SetLocalPosition(position);
			//objectTransform->SetLocalRotation(rotation);
			//objectTransform->SetLocalScale(scale);
		}

		for (const auto& object : mTreObjects)
		{
			std::string objectName = "Object_" + std::to_string(object.Id) + "_" + std::to_string(object.Category);

			Actor* objectGroupActor = CreateActor<Actor>(objectName, treObjectGroupActor);

			Transform* objectGroupTransform = objectGroupActor->GetTransform();

			R32V3 position = R32V3{ object.Position.x, object.Position.y, object.Position.z } * DEBUG_WORLD_SCALE;
			R32V3 rotation = glm::degrees(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
			R32V3 scale = R32V3{ object.Scale.x, object.Scale.y, object.Scale.z } / MAGIC_SCALE_COEFFICIENT * DEBUG_WORLD_SCALE;

			//objectGroupTransform->SetLocalPosition(position);
			//objectGroupTransform->SetLocalRotation(rotation);
			//objectGroupTransform->SetLocalScale(scale);

			Actor* objectActor = CreateActor<Actor>("Object", objectGroupActor);

			Transform* objectTransform = objectActor->GetTransform();

			objectTransform->SetLocalPosition(position);
			//objectTransform->SetLocalRotation(rotation);
			//objectTransform->SetLocalScale(scale);
		}

		for (const auto& object : mTatObjects)
		{
			std::string objectName = "Object_" + std::to_string(object.Id) + "_" + std::to_string(object.Category);

			Actor* objectGroupActor = CreateActor<Actor>(objectName, tatObjectGroupActor);

			Transform* objectGroupTransform = objectGroupActor->GetTransform();

			R32V3 position = R32V3{ object.Position.x, object.Position.y, object.Position.z } * DEBUG_WORLD_SCALE;
			R32V3 rotation = glm::degrees(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
			R32V3 scale = R32V3{ object.Scale.x, object.Scale.y, object.Scale.z } / MAGIC_SCALE_COEFFICIENT * DEBUG_WORLD_SCALE;

			//objectGroupTransform->SetLocalPosition(position);
			//objectGroupTransform->SetLocalRotation(rotation);
			//objectGroupTransform->SetLocalScale(scale);

			Actor* objectActor = CreateActor<Actor>("Object", objectGroupActor);

			Transform* objectTransform = objectActor->GetTransform();

			objectTransform->SetLocalPosition(position);
			//objectTransform->SetLocalRotation(rotation);
			//objectTransform->SetLocalScale(scale);
		}
	}
	*/

	SceneInfo LevelScene::ObjectToSceneInfo(const ObjEntry& Object)
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
		LOG(" Opening Level Scene %s\n", GetArchiveFileName().c_str());
		LOG("=============================================================\n");
		LOG("\n");
		LOG("Loading archives:\n");
		LOG("    %s\n", GetArchiveFileName().c_str());

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
		LOG("    %s\n", GetArchiveFileName().c_str());
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

		LOG("    %s\n", GetArchiveFileName().c_str());
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

		LOG("    %s\n", GetArchiveFileName().c_str());
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
