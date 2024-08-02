#include <Common/Magic.h>
#include <Common/VertexTypes.h>

#include <Common/Archive/Archive.h>

#include <Common/Model/Model.h>

#include <Common/Serializer/PlacementSerializer.h>

#include <Common/Utility/StringUtility.h>

#include <Editor/Ecs/Registry.h>
#include <Editor/Ecs/Entity.h>

#include <Editor/Ecs/Components/Transform.h>
#include <Editor/Ecs/Components/Renderable.h>

#include <Editor/Interface/Log.h>

#include <Editor/OpenGl/StaticMesh.h>

#include <Editor/Scene/LevelScene.h>
#include <Editor/Scene/SceneAssets.h>

namespace Nippon
{
	static std::map<U8, std::string> sFolderIdMapping = // TODO
	{
		//{ 0x00, "pl" },
		//{ 0x00, "em" },
		//{ 0x00, "an" },
		//{ 0x00, "gt" },
		//{ 0x00, "hm" },
		//{ 0x00, "hl" },
		//{ 0x00, "wp" },
	
		{ 0x00, "??" }, // Still unknown.. 0x52
		{ 0x01, "??" }, // Still unknown.. 0x01 0x02 0x03
		{ 0x03, "et" },
		{ 0x08, "ut" },
		{ 0x09, "es" },
		{ 0x0A, "it" },
		{ 0x0B, "vt" },
		{ 0x0C, "dr" },
		{ 0x0E, "??" }, // Still unknown..
		{ 0x0F, "us" },
	};

	LevelScene::LevelScene(ArchiveInfo const& ArchiveInfo, bool LoadFromFilePath) : Scene(ArchiveInfo, LoadFromFilePath)
	{
		mStaticGeometryEntity = GetRegistry()->CreateEntity("Static Geometry", nullptr);
	}

	LevelScene::~LevelScene()
	{

	}

	void LevelScene::Load()
	{
		AddResources();
		CreateAssets();
		BuildEntities();
	}

	void LevelScene::Save()
	{

	}

	void LevelScene::AddResources()
	{
		U32 uniqueId = GetUniqueId();

		if (LoadFromFilePath())
		{
			fs::path filePath = GetFilePath();

			AddArchiveByUniqueIdFromFilePath(uniqueId, filePath);
		}
		else
		{
			AddArchiveByUniqueId(uniqueId);
		}

		SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(uniqueId);
		Archive* levelArchive = sceneAssets->GetArchive();

		if (Archive* tscArchive = levelArchive->FindArchiveByType("TSC"))
		{
			mTscObjects = PlacementSerializer::ObjectsFromBytes(tscArchive->GetBytes(), tscArchive->GetSize());
		}

		if (Archive* treArchive = levelArchive->FindArchiveByType("TRE"))
		{
			mTreObjects = PlacementSerializer::ObjectsFromBytes(treArchive->GetBytes(), treArchive->GetSize());
		}

		if (Archive* tatArchive = levelArchive->FindArchiveByType("TAT"))
		{
			mTatObjects = PlacementSerializer::ObjectsFromBytes(tatArchive->GetBytes(), tatArchive->GetSize());
		}

		if (LoadFromFilePath())
		{
			AddEntityResources(mTscObjects);
			AddEntityResources(mTreObjects);
			AddEntityResources(mTatObjects);
		}
	}

	void LevelScene::CreateAssets()
	{
		U32 uniqueId = GetUniqueId();

		SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(uniqueId);
		Archive* levelArchive = sceneAssets->GetArchive();

		if (!sceneAssets->IsLoaded())
		{
			sceneAssets->SetLoaded();

			const auto& scrArchives = levelArchive->FindArchivesByType("SCR");
			const auto& ddsArchives = levelArchive->FindArchivesByType("DDS");

			for (const auto& archive : scrArchives)
			{
				sceneAssets->AddScrModelFromArchive(archive);
			}

			for (const auto& archive : ddsArchives)
			{
				sceneAssets->AddTextureFromArchive(archive);
			}
		}

		if (LoadFromFilePath())
		{
			CreateEntityAssets(mTscObjects);
			CreateEntityAssets(mTreObjects);
			CreateEntityAssets(mTatObjects);
		}
	}

	void LevelScene::BuildEntities()
	{
		Registry* registry = GetRegistry();
		SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(GetUniqueId());

		for (auto const& model : sceneAssets->GetModels())
		{
			Entity* modelEntity = registry->CreateEntity(model->GetName(), mStaticGeometryEntity);

			for (auto const& mesh : model->GetScrMeshes())
			{
				Entity* meshEntity = registry->CreateEntity("Mesh_" + std::to_string(mesh.Index), modelEntity);

				Transform* meshTransform = meshEntity->GetTransform();

				ScrTransform const& transform = mesh.Transform;

				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } * MAGIC_WORLD_SCALE;
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * MAGIC_WORLD_SCALE;

				meshTransform->SetPosition(position);

				for (auto const& subMesh : mesh.SubMeshes)
				{
					Entity* subMeshEntity = registry->CreateEntity("SubMesh_" + std::to_string(subMesh.Index), meshEntity);

					Transform* subMeshTransform = subMeshEntity->GetTransform();

					subMeshTransform->SetRotation(rotation);
					subMeshTransform->SetScale(scale);

					Renderable* subMeshRenderable = subMeshEntity->AttachComponent<Renderable>();
					StaticMesh* subMeshStaticMesh = subMeshRenderable->GetStaticMesh();

					std::vector<DefaultVertex> vertices = {};
					std::vector<U32> indices = {};
					
					Model::ConvertScrVertices(subMeshEntity->GetUniqueId(), subMesh.Vertices, subMesh.TextureMaps, subMesh.TextureUvs, subMesh.ColorWeights, vertices);
					Model::TriangulateScrVertices(subMesh.Vertices, indices);

					U32 textureIndex = subMesh.Header.TextureIndex;
					U32 texture = sceneAssets->GetTextureByIndex(textureIndex);

					subMeshStaticMesh->Bind();
					subMeshStaticMesh->SetVertices(vertices.data(), sizeof(DefaultVertex), vertices.size());
					subMeshStaticMesh->SetIndices(indices.data(), sizeof(U32), indices.size());
					subMeshStaticMesh->UnBind();

					subMeshRenderable->SetTexture(textureIndex, texture);

					subMeshEntity->ComputeAABB();
				}
			}
		}

		if (LoadFromFilePath())
		{
			BuildEntityEntities(mTscObjects);
			BuildEntityEntities(mTreObjects);
			BuildEntityEntities(mTatObjects);
		}
	}

	void LevelScene::AddEntityResources(std::vector<Object> const& Objects)
	{
		for (auto const& object : Objects)
		{
			std::string folderId = sFolderIdMapping[object.FolderId];
			std::string archiveId = StringUtility::ByteToHexStr(object.ArchiveId);

			U32 uniqueId = Database::GetUniqueIdByFolderIdAndArchiveId(folderId, archiveId);

			if (uniqueId)
			{
				AddArchiveByUniqueId(uniqueId);
			}
			else
			{
				Log::Add("Cannot find folderId 0x%02X with archiveId 0x%02X\n", object.FolderId, object.ArchiveId);
			}
		}
	}

	void LevelScene::CreateEntityAssets(std::vector<Object> const& Objects)
	{
		for (auto& object : Objects)
		{
			std::string folderId = sFolderIdMapping[object.FolderId];
			std::string archiveId = StringUtility::ByteToHexStr(object.ArchiveId);

			U32 uniqueId = Database::GetUniqueIdByFolderIdAndArchiveId(folderId, archiveId);

			if (uniqueId)
			{
				SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(uniqueId);
				Archive* levelArchive = sceneAssets->GetArchive();

				if (!sceneAssets->IsLoaded())
				{
					sceneAssets->SetLoaded();

					auto const& mdArchives = levelArchive->FindArchivesByType("MD");
					auto const& ddsArchives = levelArchive->FindArchivesByType("DDS");

					for (auto const& archive : mdArchives)
					{
						sceneAssets->AddMdModelFromArchive(archive, &object);
					}

					for (auto const& archive : ddsArchives)
					{
						sceneAssets->AddTextureFromArchive(archive);
					}
				}
			}
			else
			{
				Log::Add("Cannot find folderId 0x%02X with archiveId 0x%02X\n", object.FolderId, object.ArchiveId);
			}
		}
	}

	void LevelScene::BuildEntityEntities(std::vector<Object> const& Objects)
	{
		for (auto& object : Objects)
		{
			std::string folderId = sFolderIdMapping[object.FolderId];
			std::string archiveId = StringUtility::ByteToHexStr(object.ArchiveId);

			U32 uniqueId = Database::GetUniqueIdByFolderIdAndArchiveId(folderId, archiveId);

			if (uniqueId)
			{
				Registry* registry = GetRegistry();
				SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(uniqueId);

				for (auto const& model : sceneAssets->GetModels())
				{
					Entity* modelEntity = registry->CreateEntity(model->GetName(), mStaticGeometryEntity);

					for (auto const& mesh : model->GetMdMeshes())
					{
						Entity* meshEntity = registry->CreateEntity("Mesh_" + std::to_string(mesh.Index), modelEntity);

						Transform* meshTransform = meshEntity->GetTransform();

						MdTransform const& transform = mesh.Transform;

						R32V3 position = {};
						R32V3 rotation = {};
						R32V3 scale = {};

						if (Object const* object = model->GetObjectRef())
						{
							position = R32V3{ object->Position.x, object->Position.y, object->Position.z } * MAGIC_WORLD_SCALE;
							rotation = R32V3{ object->Rotation.x, object->Rotation.y, object->Rotation.z } + R32V3{ MAGIC_ENTITY_TO_LEVEL_PITCH_OFFSET, MAGIC_ENTITY_TO_LEVEL_YAW_OFFSET, MAGIC_ENTITY_TO_LEVEL_ROLL_OFFSET };
							scale = R32V3{ object->Scale.x, object->Scale.y, object->Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;
						}
						else
						{
							R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } * MAGIC_WORLD_SCALE;
							R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
							R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * MAGIC_WORLD_SCALE;
						}

						meshTransform->SetPosition(position);

						for (auto const& subMesh : mesh.SubMeshes)
						{
							Entity* subMeshEntity = registry->CreateEntity("SubMesh_" + std::to_string(subMesh.Index), meshEntity);

							Transform* subMeshTransform = subMeshEntity->GetTransform();

							subMeshTransform->SetRotation(rotation);
							subMeshTransform->SetScale(scale);

							Renderable* subMeshRenderable = subMeshEntity->AttachComponent<Renderable>();
							StaticMesh* subMeshStaticMesh = subMeshRenderable->GetStaticMesh();

							std::vector<DefaultVertex> vertices = {};
							std::vector<U32> indices = {};

							Model::ConvertMdVertices(subMeshEntity->GetUniqueId(), subMesh.Vertices, subMesh.TextureMaps, subMesh.TextureUvs, subMesh.ColorWeights, vertices);
							Model::TriangulateMdVertices(subMesh.Vertices, indices);

							U32 textureIndex = subMesh.Header.TextureIndex;
							U32 texture = sceneAssets->GetTextureByIndex(textureIndex);

							subMeshStaticMesh->Bind();
							subMeshStaticMesh->SetVertices(vertices.data(), sizeof(DefaultVertex), vertices.size());
							subMeshStaticMesh->SetIndices(indices.data(), sizeof(U32), indices.size());
							subMeshStaticMesh->UnBind();

							subMeshRenderable->SetTexture(textureIndex, texture);

							subMeshEntity->ComputeAABB();
						}
					}
				}
			}
			else
			{
				Log::Add("Cannot find folderId 0x%02X with archiveId 0x%02X\n", object.FolderId, object.ArchiveId);
			}
		}
	}
}