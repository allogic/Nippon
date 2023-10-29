#include <Common/Macros.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>

#include <Editor/Actor.h>
#include <Editor/Editor.h>
#include <Editor/Texture2D.h>
#include <Editor/Magic.h>

#include <Editor/Databases/FileDatabase.h>

#include <Editor/Scenes/EntityScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Serializer/ModelSerializer.h>

namespace ark
{
	EntityScene::EntityScene(const FileContainer* FileContainer) : Scene{ FileContainer }
	{
		mEntityGeometryActor = CreateActor<Actor>("Entity Geometry", nullptr);
	}

	EntityScene::~EntityScene()
	{
		
	}

	void EntityScene::Load()
	{
		AddResources();
		CreateAssets();
		BuildActors();
	}

	void EntityScene::Save()
	{

	}

	void EntityScene::AddResources()
	{
		U32 identifier = GetFileContainer()->GetIdentifier();

		Archive* datArchive = AddResource(identifier, GetFileContainer()->GetDatFile());
	}

	void EntityScene::CreateAssets()
	{
		U32 identifier = GetFileContainer()->GetIdentifier();

		SceneResource* sceneResource = GetSceneResourceByIdentifier(identifier);

		const auto& mdArchives = sceneResource->GetArchive()->FindArchivesByType("MD");
		const auto& ddsArchives = sceneResource->GetArchive()->FindArchivesByType("DDS");

		for (const auto& archive : mdArchives)
		{
			sceneResource->AddMdModelFromArchive(archive, nullptr);
		}

		for (const auto& archive : ddsArchives)
		{
			sceneResource->AddTextureFromArchive(archive);
		}
	}

	void EntityScene::BuildActors()
	{
		U32 identifier = GetFileContainer()->GetIdentifier();

		SceneResource* sceneResource = GetSceneResourceByIdentifier(identifier);

		for (const auto& model : sceneResource->GetModels())
		{
			Actor* modelActor = CreateActor<Actor>(model.GetName(), mEntityGeometryActor);

			for (const auto& mesh : model.GetMdMeshes())
			{
				Actor* meshActor = CreateActor<Actor>("Mesh_" + std::to_string(mesh.Index), modelActor);

				Transform* meshTransform = meshActor->GetTransform();

				const MdTransform& transform = mesh.Transform;

				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };

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
