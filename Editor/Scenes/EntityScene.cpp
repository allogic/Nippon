#include <Common/Macros.h>
#include <Common/BlowFish.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>

#include <Editor/Actor.h>
#include <Editor/Editor.h>
#include <Editor/Texture2D.h>
#include <Editor/Magic.h>

#include <Editor/Scenes/EntityScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Serializer/MdSerializer.h>

#include <Editor/Utilities/TextureUtils.h>

namespace ark
{
	EntityScene::EntityScene(const SceneInfo& Info) : Scene{ Info }
	{
		mEntityGeometryActor = CreateActor<Actor>("Entity Geometry", nullptr);
	}

	EntityScene::~EntityScene()
	{
		Texture2D::Destroy(mMdTextures);

		if (mDatArchive)
		{
			delete mDatArchive;
		}
	}

	void EntityScene::Load()
	{
		LoadArchive();
		LoadEntity();

		AddStaticGeometry();
		
		if (mEnableConsole)
		{
			PrintSummary();
		}
	}

	void EntityScene::Save()
	{

	}

	void EntityScene::LoadArchive()
	{
		fs::path relativeDatFile = fs::path{ GetGroupKey() } / GetDatArchiveFileName();

		fs::path absoluteDatFile = gDataDir / relativeDatFile;

		std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFile);

		gBlowFish->Decrypt(datBytes);

		mDatArchive = new Archive;

		mDatArchive->DeSerialize(datBytes);
	}

	void EntityScene::LoadEntity()
	{
		mDatArchive->FindArchivesByType("MD", mMdArchives);
		mDatArchive->FindArchivesByType("DDS", mDdsArchives);

		for (const auto& archive : mMdArchives)
		{
			MdGroup& group = mMdGroups.emplace_back(MdSerializer::FromBytes(archive->GetBytes(), archive->GetSize()));

			group.Name = archive->GetName();
		}

		for (const auto& archive : mDdsArchives)
		{
			mMdTextures.emplace_back(TextureUtils::ReadDDS(archive->GetBytes(), archive->GetSize()));
		}
	}

	void EntityScene::AddStaticGeometry()
	{
		for (const auto& group : mMdGroups)
		{
			Actor* groupActor = CreateActor<Actor>(group.Name, mEntityGeometryActor);

			for (const auto& model : group.Models)
			{
				Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

				Transform* modelTransform = modelActor->GetTransform();

				const MdTransform& transform = model.Transform;

				R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
				R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
				R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };

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
					U32 texture = (textureIndex < mMdTextures.size()) ? mMdTextures[textureIndex] : 0;

					divisionRenderable->SetVertexBuffer(vertices);
					divisionRenderable->SetElementBuffer(elements);
					divisionRenderable->LocalToRemote();
					divisionRenderable->SetTexture(textureIndex, texture);

					divisonActor->ComputeAABB();
				}
			}
		}
	}

	void EntityScene::PrintSummary()
	{
		LOG("\n");
		LOG(" Opening Entity Scene \\%s\\%s\n", GetGroupKey().c_str(), GetSceneKey().c_str());
		LOG("-----------------------------------------------------------------------------------------\n");
		LOG("\n");
		LOG("Loading archives:\n");
		LOG("    %s\n", GetDatArchiveFileName().c_str());

		LOG("\n");
		LOG("Searching files:\n");
		LOG("    %s\n", GetDatArchiveFileName().c_str());
		LOG("        Found %u MD files\n", (U32)mMdArchives.size());
		LOG("        Found %u DDS files\n", (U32)mDdsArchives.size());

		LOG("\n");
		LOG("Loading models:\n");

		for (const auto& archive : mMdArchives)
		{
			LOG("    %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
		}

		LOG("\n");
		LOG("Loading textures:\n");

		for (const auto& archive : mDdsArchives)
		{
			LOG("    %s.%s\n", archive->GetName().c_str(), archive->GetType().c_str());
		}

		LOG("\n");
	}
}
