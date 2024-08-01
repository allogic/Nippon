#include <Common/Magic.h>
#include <Common/VertexTypes.h>

#include <Common/Archive/Archive.h>

#include <Common/Model/Model.h>

#include <Editor/Ecs/Registry.h>
#include <Editor/Ecs/Entity.h>

#include <Editor/Ecs/Components/Transform.h>
#include <Editor/Ecs/Components/Renderable.h>

#include <Editor/OpenGl/StaticMesh.h>

#include <Editor/Scene/EntityScene.h>
#include <Editor/Scene/SceneAssets.h>

namespace Nippon
{
	EntityScene::EntityScene(ArchiveInfo const& ArchiveInfo) : Scene(ArchiveInfo)
	{
		mStaticGeometryEntity = GetRegistry()->CreateEntity("Static Geometry", nullptr);
	}

	EntityScene::~EntityScene()
	{

	}

	void EntityScene::Load()
	{
		AddResources();
		CreateAssets();
		BuildEntities();
	}

	void EntityScene::Save()
	{

	}

	void EntityScene::AddResources()
	{
		AddArchiveByUniqueId(GetUniqueId());
	}

	void EntityScene::CreateAssets()
	{		
		SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(GetUniqueId());
		Archive* entityArchive = sceneAssets->GetArchive();
		
		if (!sceneAssets->IsLoaded())
		{
			sceneAssets->SetLoaded();

			auto const& mdArchives = entityArchive->FindArchivesByType("MD");
			auto const& ddsArchives = entityArchive->FindArchivesByType("DDS");

			for (auto const& mdArchive : mdArchives)
			{
				sceneAssets->AddMdModelFromArchive(mdArchive, nullptr);
			}

			for (auto const& ddsArchive : ddsArchives)
			{
				sceneAssets->AddTextureFromArchive(ddsArchive);
			}
		}
	}

	void EntityScene::BuildEntities()
	{
		Registry* registry = GetRegistry();
		SceneAssets* sceneAssets = GetSceneAssetsByUniqueId(GetUniqueId());
		
		for (auto const& model : sceneAssets->GetModels())
		{
			Entity* modelEntity = registry->CreateEntity(model->GetName(), mStaticGeometryEntity);
		
			for (auto const& mesh : model->GetMdMeshes())
			{
				Entity* meshEntity = registry->CreateEntity("Mesh_" + std::to_string(mesh.Index), modelEntity);
		
				Transform* meshTransform = meshEntity->GetTransform();
		
				MdTransform const& transform = mesh.Transform;
		
				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };
		
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
}