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

#include <Editor/Utilities/ImageUtils.h>

namespace ark
{
	EntityScene::EntityScene(const FileContainer* FileContainer) : Scene{ FileContainer }
	{
		mEntityGeometryActor = CreateActor<Actor>("Entity Geometry", nullptr);
	}

	EntityScene::~EntityScene()
	{
		if (mDatArchive)
		{
			delete mDatArchive;
		}
	}

	void EntityScene::Load()
	{
		LoadArchives();
		LoadAssets();

		AddStaticGeometry();
		
		if (mEnableConsole)
		{
			PrintSummary();
		}
	}

	void EntityScene::Save()
	{

	}

	void EntityScene::LoadArchives()
	{
		fs::path datFile = gDataDir / GetFileContainer()->GetDatFile().GetRelativeFile();

		std::vector<U8> datBytes = FsUtils::ReadBinary(datFile);

		gBlowFish->Decrypt(datBytes);

		mDatArchive = new Archive;

		mDatArchive->DeSerialize(datBytes);

		mDatArchive->FindArchivesByType("MD", mMdArchives);
		mDatArchive->FindArchivesByType("DDS", mDdsArchives);
	}

	void EntityScene::LoadAssets()
	{
		for (const auto& archive : mMdArchives)
		{
			GenericModel& model = AddModel();

			model.SetType(GenericModel::eModelTypeMd);
			model.SetName(archive->GetName());
			model.SetMdMeshes(ModelSerializer::DeSerialize<MdMesh>(archive->GetBytes(), archive->GetSize()));
		}
		
		for (const auto& archive : mDdsArchives)
		{
			GenericTexture& texture = AddTexture();

			texture.SetType(GenericTexture::eTextureTypeMd);
			texture.SetName(archive->GetName());
			texture.SetTexture(ImageUtils::ReadDDS(archive->GetBytes(), archive->GetSize()));
		}
	}

	void EntityScene::AddStaticGeometry()
	{
		const auto& models = GetModels();
		const auto& textures = GetTextures();

		for (const auto& model : models)
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
					U32 texture = (textureIndex < textures.size()) ? textures[textureIndex].GetTexture() : 0;

					subMeshRenderable->SetVertexBuffer(vertices);
					subMeshRenderable->SetElementBuffer(elements);
					subMeshRenderable->LocalToRemote();
					subMeshRenderable->SetTexture(textureIndex, texture);

					subMeshActor->ComputeAABB();
				}
			}
		}
	}

	void EntityScene::PrintSummary()
	{
		LOG("\n");
		LOG(" Opening Entity Scene \\%s\\%s\n", GetFileContainer()->GetDirectoryId(), GetFileContainer()->GetFileId());
		LOG("-----------------------------------------------------------------------------------------\n");
		LOG("\n");
		LOG("Loading archives:\n");
		LOG("    %s\n", GetFileContainer()->GetDatFile().GetRelativeFile());

		LOG("\n");
		LOG("Searching files:\n");
		LOG("    %s\n", GetFileContainer()->GetDatFile().GetRelativeFile());
		LOG("        Found %u MD files\n", (U32)mMdArchives.size());
		LOG("        Found %u DDS files\n", (U32)mDdsArchives.size());

		LOG("\n");
		LOG("Loading models:\n");

		for (const auto& archive : mMdArchives)
		{
			LOG("    %s.%s\n", archive->GetName(), archive->GetType());
		}

		LOG("\n");
		LOG("Loading textures:\n");

		for (const auto& archive : mDdsArchives)
		{
			LOG("    %s.%s\n", archive->GetName(), archive->GetType());
		}

		LOG("\n");
	}
}
