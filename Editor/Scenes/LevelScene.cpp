#include <Common/Macros.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>

#include <Editor/Actor.h>
#include <Editor/Editor.h>
#include <Editor/Texture2D.h>
#include <Editor/Magic.h>

#include <Editor/Databases/FileDatabase.h>

#include <Editor/Scenes/LevelScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Serializer/ModelSerializer.h>
#include <Editor/Serializer/ObjectSerializer.h>

namespace ark
{
	static std::map<U8, U16> sDirectoryByCategory =
	{
		//{ 0x00, (('p' << 8) | 'l') },
		//{ 0x00, (('e' << 8) | 'm') },
		//{ 0x00, (('a' << 8) | 'n') },
		//{ 0x00, (('g' << 8) | 't') },
		//{ 0x00, (('h' << 8) | 'm') },
		//{ 0x00, (('h' << 8) | 'l') },
		//{ 0x00, (('w' << 8) | 'p') },

		{ 0x00, (('?' << 8) | '?') }, // Still unknown.. 0x52
		{ 0x01, (('?' << 8) | '?') }, // Still unknown.. 0x01 0x02 0x03
		{ 0x03, (('e' << 8) | 't') },
		{ 0x08, (('u' << 8) | 't') },
		{ 0x09, (('e' << 8) | 's') },
		{ 0x0A, (('i' << 8) | 't') },
		{ 0x0B, (('v' << 8) | 't') },
		{ 0x0C, (('d' << 8) | 'r') },
		{ 0x0E, (('?' << 8) | '?') }, // Still unknown..
		{ 0x0F, (('u' << 8) | 's') },
	};

	LevelScene::LevelScene(const FileContainer* FileContainer) : Scene{ FileContainer }
	{
		mLevelGeometryActor = CreateActor<Actor>("Level Geometry", nullptr);
		mEntityGeometryActor = CreateActor<Actor>("Entity Geometry", nullptr);
	}

	LevelScene::~LevelScene()
	{

	}

	void LevelScene::Load()
	{
		AddResources();
		CreateAssets();
		BuildActors();
	}

	void LevelScene::Save()
	{

	}

	void LevelScene::AddResources()
	{
		U32 identifier = GetFileContainer()->GetIdentifier();

		Archive* datArchive = AddResource(identifier, GetFileContainer()->GetDatFile());

		Archive* tscArchive = datArchive->FindArchiveByType("TSC");
		Archive* treArchive = datArchive->FindArchiveByType("TRE");
		Archive* tatArchive = datArchive->FindArchiveByType("TAT");

		mTscObjects = ObjectSerializer::DeSerialize(tscArchive->GetBytes(), tscArchive->GetSize());
		mTreObjects = ObjectSerializer::DeSerialize(treArchive->GetBytes(), treArchive->GetSize());
		mTatObjects = ObjectSerializer::DeSerialize(tatArchive->GetBytes(), tatArchive->GetSize());

		AddEntityResources(mTscObjects);
		AddEntityResources(mTreObjects);
		AddEntityResources(mTatObjects);

		for (const auto& [cat, ids] : mTmp)
		{
			LOG("0x%02X -> ", cat);
			for (const auto& id : ids)
			{
				LOG("0x%02X ", id);
			}
			LOG("\n");
		}
		LOG("\n");
	}

	void LevelScene::CreateAssets()
	{
		U32 identifier = GetFileContainer()->GetIdentifier();

		SceneResource* sceneResource = GetSceneResourceByIdentifier(identifier);

		const auto& scrArchives = sceneResource->GetArchive()->FindArchivesByType("SCR");
		const auto& ddsArchives = sceneResource->GetArchive()->FindArchivesByType("DDS");

		for (const auto& archive : scrArchives)
		{
			sceneResource->AddScrModelFromArchive(archive);
		}

		for (const auto& archive : ddsArchives)
		{
			sceneResource->AddTextureFromArchive(archive);
		}

		CreateEntityAssets(mTscObjects);
		CreateEntityAssets(mTreObjects);
		CreateEntityAssets(mTatObjects);
	}

	void LevelScene::BuildActors()
	{
		U32 identifier = GetFileContainer()->GetIdentifier();

		SceneResource* sceneResource = GetSceneResourceByIdentifier(identifier);

		for (const auto& model : sceneResource->GetModels())
		{
			Actor* modelActor = CreateActor<Actor>(model.GetName(), mLevelGeometryActor);

			for (const auto& mesh : model.GetScrMeshes())
			{
				Actor* meshActor = CreateActor<Actor>("Mesh_" + std::to_string(mesh.Index), modelActor);

				Transform* meshTransform = meshActor->GetTransform();

				const ScrTransform& transform = mesh.Transform;

				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } * MAGIC_WORLD_SCALE;
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * MAGIC_WORLD_SCALE;

				meshTransform->SetLocalPosition(position);

				for (const auto& subMesh : mesh.SubMeshes)
				{
					Actor* subMeshActor = CreateActor<Actor>("SubMesh_" + std::to_string(subMesh.Index), meshActor);

					Transform* subMeshTransform = subMeshActor->GetComponent<Transform>();

					subMeshTransform->SetLocalRotation(rotation);
					subMeshTransform->SetLocalScale(scale);

					Renderable* subMeshRenderable = subMeshActor->AttachComponent<Renderable>();

					std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(subMeshActor, subMesh.Vertices, subMesh.TextureMaps, subMesh.TextureUvs, subMesh.ColorWeights);
					std::vector<U32> elements = ElementConverter::ToElementBuffer(subMesh.Vertices);

					U32 textureIndex = subMesh.Header.TextureIndex;
					U32 texture = sceneResource->GetTexturesByIndex(textureIndex);

					subMeshRenderable->SetVertexBuffer(vertices);
					subMeshRenderable->SetElementBuffer(elements);
					subMeshRenderable->LocalToRemote();
					subMeshRenderable->SetTexture(textureIndex, texture);

					subMeshActor->ComputeAABB();
				}
			}
		}

		BuildEntityActors(mTscObjects);
		BuildEntityActors(mTreObjects);
		BuildEntityActors(mTatObjects);
	}

	void LevelScene::AddEntityResources(const std::vector<Object>& Objects)
	{
		for (const auto& object : Objects)
		{
			const auto findIt = sDirectoryByCategory.find(object.Category);

			if (findIt == sDirectoryByCategory.end())
			{
				//LOG("Cannot find category 0x%02X with id 0x%02X\n", object.Category, object.Id);

				mTmp[object.Category].emplace(object.Id);
			}
			else
			{
				U32 identifier = (sDirectoryByCategory[object.Category] << 16) | object.Id;

				const FileContainer* fileContainer = FileDatabase::GetFileContainerByIdentifier(identifier);

				if (fileContainer)
				{
					Archive* archive = AddResource(fileContainer->GetIdentifier(), fileContainer->GetDatFile());
				}
			}
		}
	}

	void LevelScene::CreateEntityAssets(const std::vector<Object>& Objects)
	{
		for (auto& object : Objects)
		{
			const auto findIt = sDirectoryByCategory.find(object.Category);

			if (findIt == sDirectoryByCategory.end())
			{
				//LOG("Cannot find category 0x%02X with id 0x%02X\n", object.Category, object.Id);
			}
			else
			{
				U32 identifier = (sDirectoryByCategory[object.Category] << 16) | object.Id;

				SceneResource* sceneResource = GetSceneResourceByIdentifier(identifier);

				if (sceneResource && !sceneResource->IsLoaded())
				{
					const auto& mdArchives = sceneResource->GetArchive()->FindArchivesByType("MD");
					const auto& ddsArchives = sceneResource->GetArchive()->FindArchivesByType("DDS");

					for (const auto& archive : mdArchives)
					{
						sceneResource->AddMdModelFromArchive(archive, &object);
					}

					for (const auto& archive : ddsArchives)
					{
						sceneResource->AddTextureFromArchive(archive);
					}

					sceneResource->SetLoaded(true);
				}
			}
		}
	}

	void LevelScene::BuildEntityActors(const std::vector<Object>& Objects)
	{
		for (const auto& object : Objects)
		{
			const auto findIt = sDirectoryByCategory.find(object.Category);

			if (findIt == sDirectoryByCategory.end())
			{
				//LOG("Cannot find category 0x%02X with id 0x%02X\n", object.Category, object.Id);
			}
			else
			{
				U32 identifier = (sDirectoryByCategory[object.Category] << 16) | object.Id;

				SceneResource* sceneResource = GetSceneResourceByIdentifier(identifier);

				if (sceneResource && sceneResource->IsLoaded())
				{
					for (const auto& model : sceneResource->GetModels())
					{
						Actor* modelActor = CreateActor<Actor>(model.GetName(), mEntityGeometryActor);

						for (const auto& mesh : model.GetMdMeshes())
						{
							Actor* meshActor = CreateActor<Actor>("Mesh_" + std::to_string(mesh.Index), modelActor);

							Transform* meshTransform = meshActor->GetTransform();

							const MdTransform& transform = mesh.Transform;

							R32V3 position = {};
							R32V3 rotation = {};
							R32V3 scale = {};

							if (const Object* object = model.GetObjectRef())
							{
								position = R32V3{ object->Position.x, object->Position.y, object->Position.z } *MAGIC_WORLD_SCALE;
								rotation = R32V3{ object->Rotation.x, object->Rotation.y, object->Rotation.z } + R32V3{ MAGIC_ENTITY_TO_LEVEL_PITCH_OFFSET, MAGIC_ENTITY_TO_LEVEL_YAW_OFFSET, MAGIC_ENTITY_TO_LEVEL_ROLL_OFFSET };
								scale = R32V3{ object->Scale.x, object->Scale.y, object->Scale.z } / MAGIC_ENTITY_TO_LEVEL_SCALE_COEFFICIENT;
							}
							else
							{
								R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } *MAGIC_WORLD_SCALE;
								R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
								R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * MAGIC_WORLD_SCALE;
							}

							meshTransform->SetLocalPosition(position);

							for (const auto& subMesh : mesh.SubMeshes)
							{
								Actor* subMeshActor = CreateActor<Actor>("SubMesh_" + std::to_string(subMesh.Index), meshActor);

								Transform* subMeshTransform = subMeshActor->GetComponent<Transform>();

								subMeshTransform->SetLocalRotation(rotation);
								subMeshTransform->SetLocalScale(scale);

								Renderable* subMeshRenderable = subMeshActor->AttachComponent<Renderable>();

								std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(subMeshActor, subMesh.Vertices, subMesh.TextureMaps, subMesh.TextureUvs, subMesh.ColorWeights);
								std::vector<U32> elements = ElementConverter::ToElementBuffer(subMesh.Vertices);

								U32 textureIndex = subMesh.Header.TextureIndex;
								U32 texture = sceneResource->GetTexturesByIndex(textureIndex);

								subMeshRenderable->SetVertexBuffer(vertices);
								subMeshRenderable->SetElementBuffer(elements);
								subMeshRenderable->LocalToRemote();
								subMeshRenderable->SetTexture(textureIndex, texture);

								subMeshActor->ComputeAABB();
							}
						}
					}
				}
			}
		}
	}
}
