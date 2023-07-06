#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Editor.h>

#include <Editor/Scenes/LevelScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Interface/Outline.h>

#include <Editor/Serializer/ObjSerializer.h>
#include <Editor/Serializer/ScrSerializer.h>
#include <Editor/Serializer/MdSerializer.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	LevelScene::LevelScene(
		const std::string& Entry,
		const std::string& SubEntry)
		: Scene{ eSceneTypeLevel, Entry, SubEntry }
	{
		Load();
	}

	LevelScene::LevelScene(
		const std::string& Entry,
		const std::string& SubEntry,
		const std::string& SceneName,
		const std::string& WindowName)
		: Scene{ eSceneTypeLevel, Entry, SubEntry, SceneName, WindowName }
	{
		Load();
	}

	LevelScene::~LevelScene()
	{
		Save();

		// TODO: Delete textures
	}

	void LevelScene::Load()
	{
		std::string mapId = StringUtils::CutFront(mEntry, 2);

		fs::path lvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mEntry / mSubEntry;
		fs::path datDir = lvlDir / fs::path{ "r" + mapId + mSubEntry + "@dat" };

		fs::path tscFile = FsUtils::SearchFileByType(datDir, "TSC");
		fs::path treFile = FsUtils::SearchFileByType(datDir, "TRE");
		fs::path tatFile = FsUtils::SearchFileByType(datDir, "TAT");

		//std::vector<ObjEntry> objects = {};
		//
		//if (fs::exists(tscFile))
		//{
		//	auto tsc = ObjSerializer::FromFile(tscFile);
		//
		//	objects.insert(objects.end(), tsc.begin(), tsc.end());
		//}
		//
		//if (fs::exists(treFile))
		//{
		//	auto tre = ObjSerializer::FromFile(treFile);
		//
		//	objects.insert(objects.end(), tre.begin(), tre.end());
		//}
		//
		//if (fs::exists(tatFile))
		//{
		//	auto tat = ObjSerializer::FromFile(tatFile);
		//
		//	objects.insert(objects.end(), tat.begin(), tat.end());
		//}

		auto scrFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "SCR");
		auto ddsFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "DDS");

		for (const auto& file : ddsFiles)
		{
			mScrTextures.emplace_back(TextureUtils::LoadDirectDrawSurface(file));
		}

		for (const auto& file : scrFiles)
		{
			AddStaticGeometry(ScrSerializer::FromFile(file));
		}

		//LoadMd(objects);
	}

	void LevelScene::Save()
	{
		
	}

	void LevelScene::LoadMd(const std::vector<ObjEntry>& Objects)
	{
		//std::set<std::string> categories = {};
		//std::set<std::string> ids = {};
		//
		//for (const auto& object : Objects)
		//{
		//	std::string entry = StringUtils::ByteToString(object.Category);
		//	std::string subEntry = StringUtils::ByteToString(object.Id);
		//
		//	if (object.Category == 0)
		//	{
		//		if (object.Id == 0)
		//		{
		//			entry = "es";
		//			subEntry = "00";
		//
		//			fs::path lvlDir = fs::path{ gConfig["unpackDir"].GetString() } / entry / subEntry;
		//			fs::path datDir = lvlDir / fs::path{ entry + subEntry + "@dat" };
		//
		//			fs::path mdFile = FsUtils::SearchFileByType(datDir, "MD");
		//			auto ddsFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "DDS");
		//
		//			auto models = MdSerializer::FromFile(mdFile);
		//
		//			MdModelGroup mdModelGroup = {};
		//
		//			mdModelGroup.Models.insert(mdModelGroup.Models.begin(), models.begin(), models.end());
		//
		//			for (const auto& file : ddsFiles)
		//			{
		//				mdModelGroup.Textures.emplace_back(TextureUtils::LoadDirectDrawSurface(file));
		//			}
		//
		//			mMdModelGroups.emplace_back(mdModelGroup);
		//		}
		//
		//		LOG("Loading /%s/%s\n", entry.c_str(), subEntry.c_str());
		//	}
		//}
		//
		//LOG("\n");
		//
		//LOG("Categories\n");
		//for (const auto& category : categories)
		//{
		//	LOG("  %s\n", category.c_str());
		//}
		//
		//LOG("\n");
		//
		//LOG("Ids\n");
		//for (const auto& id : ids)
		//{
		//	LOG("  %s\n", id.c_str());
		//}
	}

	void LevelScene::AddStaticGeometry(const ScrGroup& Group)
	{
		Actor* groupActor = CreateActor<Actor>(Group.Name, mStaticGeometryActor);

		for (const auto& model : Group.Models)
		{
			Actor* modelActor = CreateActor<Actor>("Model_" + std::to_string(model.Index), groupActor);

			Transform* modelTransform = modelActor->GetTransform();

			const ScrTransform& transform = model.Transform;

			R32V3 position = R32V3{ transform.Position.x, transform.Position.y, transform.Position.z } * DEBUG_WORLD_SCALE;
			R32V3 rotation = glm::degrees(R32V3{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z } / 360.0F * MAGIC_ROTATION_COEFFICIENT);
			R32V3 scale = R32V3{ transform.Scale.x, transform.Scale.y, transform.Scale.z } / MAGIC_SCALE_COEFFICIENT * DEBUG_WORLD_SCALE;

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

				Texture2D* texture = (textureIndex < mScrTextures.size()) ? mScrTextures[textureIndex] : nullptr;

				divisionRenderable->SetVertexBuffer(vertices);
				divisionRenderable->SetElementBuffer(elements);
				divisionRenderable->LocalToRemote();
				divisionRenderable->SetTexture(textureIndex, texture);

				divisonActor->ComputeAxisAlignedBoundingBoxRecursive();
			}
		}
	}
}