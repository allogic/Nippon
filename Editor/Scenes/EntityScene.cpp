#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Editor.h>

#include <Editor/Scenes/EntityScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Interface/Outline.h>

#include <Editor/Serializer/MdSerializer.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	EntityScene::EntityScene(
		const std::string& Entry,
		const std::string& SubEntry)
		: Scene{ eSceneTypeEntity, Entry, SubEntry }
	{
		Load();
	}

	EntityScene::EntityScene(
		const std::string& Entry,
		const std::string& SubEntry,
		const std::string& SceneName,
		const std::string& WindowName)
		: Scene{ eSceneTypeEntity, Entry, SubEntry, SceneName, WindowName }
	{
		Load();
	}

	EntityScene::~EntityScene()
	{
		Save();

		// TODO: Delete textures
	}

	void EntityScene::Load()
	{
		fs::path lvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mEntry / mSubEntry;
		fs::path datDir = lvlDir / fs::path{ mEntry + mSubEntry + "@dat" };

		fs::path mdFile = FsUtils::SearchFileByType(datDir, "MD");
		auto ddsFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "DDS");

		for (const auto& file : ddsFiles)
		{
			mMdTextures.emplace_back(TextureUtils::LoadDirectDrawSurface(file));
		}

		if (fs::exists(mdFile))
		{
			AddStaticGeometry(MdSerializer::FromFile(mdFile));
		}
	}

	void EntityScene::Save()
	{

	}

	void EntityScene::AddStaticGeometry(const MdGroup& Group)
	{
		Actor* groupActor = CreateActor<Actor>(Group.Name, mStaticGeometryActor);

		for (const auto& model : Group.Models)
		{
			Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

			Transform* modelTransform = modelActor->GetTransform();

			const MdTransform& transform = model.Transform;

			R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z };
			R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
			R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z };

			//modelTransform->SetLocalPosition(position);
			modelTransform->SetLocalRotation(rotation);
			modelTransform->SetLocalScale(scale);

			for (const auto& division : model.Entry.Divisions)
			{
				Actor* divisonActor = CreateActor<Actor>("Division_" + std::to_string(division.Index), modelActor);

				Transform* divisionTransform = divisonActor->GetComponent<Transform>();

				divisionTransform->SetLocalPosition(position);
				//divisionTransform->SetLocalRotation(rotation);
				//divisionTransform->SetLocalScale(scale);

				Renderable* divisionRenderable = divisonActor->AttachComponent<Renderable>();

				std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
				std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);

				U32 textureIndex = division.Header.TextureIndex;

				Texture2D* texture = (textureIndex < mMdTextures.size()) ? mMdTextures[textureIndex] : nullptr;

				divisionRenderable->SetVertexBuffer(vertices);
				divisionRenderable->SetElementBuffer(elements);
				divisionRenderable->LocalToRemote();
				divisionRenderable->SetTexture(textureIndex, texture);

				divisonActor->ComputeAxisAlignedBoundingBoxRecursive();
			}
		}
	}
}