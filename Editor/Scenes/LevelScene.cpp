#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Scenes/LevelScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Interface/Outline.h>

#include <Editor/Serializer/ObjSerializer.h>
#include <Editor/Serializer/ScrSerializer.h>

#include <Vendor/rapidjson/document.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

extern std::map<std::string, ark::LevelScene*> gLevelScenes;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  LevelScene::LevelScene(
    const std::string& Entry,
    const std::string& SubEntry,
    const std::string& SceneName,
    const std::string& WindowName)
    : Scene{ eSceneTypeLevel, Entry, SubEntry, SceneName, WindowName }
  {
    Load();

    ModelsToActors();
    ObjectsToActors();
  }

  LevelScene::~LevelScene()
  {
    Save();
  }

  void LevelScene::Load()
  {
    std::string mapId = StringUtils::CutFront(mEntry, 2);

    fs::path lvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mEntry / mSubEntry;
    fs::path datDir = lvlDir / fs::path{ "r" + mapId + mSubEntry + ".dat" };

    fs::path tscFile = FsUtils::SearchFileByType(datDir, "TSC");
    fs::path treFile = FsUtils::SearchFileByType(datDir, "TRE");
    fs::path tatFile = FsUtils::SearchFileByType(datDir, "TAT");

    auto tsc = ObjSerializer::FromFile(tscFile);
    auto tre = ObjSerializer::FromFile(treFile);
    auto tat = ObjSerializer::FromFile(tatFile);

    mObjects.insert(mObjects.end(), tsc.begin(), tsc.end());
    mObjects.insert(mObjects.end(), tre.begin(), tre.end());
    mObjects.insert(mObjects.end(), tat.begin(), tat.end());

    auto scrFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "SCR");
    auto ddsFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "DDS");

    for (const auto& file : scrFiles)
    {
      auto models = ScrSerializer::FromFile(file);

      mModels.insert(mModels.end(), models.begin(), models.end());
    }

    for (const auto& file : ddsFiles)
    {
      mTextures.emplace_back(TextureUtils::LoadDDS(file));
    }
  }

  void LevelScene::Save()
  {
    
  }

  void LevelScene::ModelsToActors()
  {
    for (const auto& [model, trans] : mModels)
    {
      Actor* modelActor = CreateActor<Actor>(model.Name, nullptr);

      Transform* transform = modelActor->GetTransform();

      transform->SetLocalPosition(R32V3{ trans.Position.x, trans.Position.y, trans.Position.z });
      transform->SetLocalRotation(glm::degrees(R32V3{ trans.Rotation.x, trans.Rotation.y, trans.Rotation.z } / 360.0F));
      transform->SetLocalScale(R32V3{ trans.Scale.x, trans.Scale.y, trans.Scale.z } / 100000.0F);

      for (const auto& division : model.Divisions)
      {
        Actor* childActor = CreateActor<Actor>("Division", modelActor);

        Renderable* renderable = childActor->AttachComponent<Renderable>();

        std::vector<DefaultVertex> vertices = VertexConverter::ToVertexBuffer(division.Vertices, division.TextureMaps, division.TextureUvs, division.ColorWeights);
        std::vector<U32> elements = ElementConverter::ToElementBuffer(division.Vertices);
        Texture2D* texture = (division.Header.TextureIndex < mTextures.size()) ? mTextures[division.Header.TextureIndex] : nullptr;

        renderable->SetVertexBuffer(vertices);
        renderable->SetElementBuffer(elements);
        renderable->LocalToRemote();
        renderable->SetTexture(texture);

        AABB aabb = Math::ComputeBoundingBox(vertices, transform->GetLocalScale());

        childActor->SetAABB(aabb);
      }
    }
  }

  void LevelScene::ObjectsToActors()
  {
    for (const auto& object : mObjects)
    {
      Actor* objActor = CreateActor<Actor>("Object", nullptr);
    
      Transform* objTransform = objActor->GetTransform();
    
      objTransform->SetLocalPosition(R32V3{ object.Position.x, object.Position.y, object.Position.z });
      objTransform->SetLocalRotation(R32V3{ object.Rotation.x, object.Rotation.y, object.Rotation.z } / 255.0F);
      objTransform->SetLocalScale(R32V3{ object.Scale.x, object.Scale.y, object.Scale.z } / 100000.0F);
    }
  }
}