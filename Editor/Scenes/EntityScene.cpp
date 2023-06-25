#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Scenes/EntityScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Interface/Outline.h>

#include <Editor/Serializer/MdSerializer.h>

#include <Vendor/rapidjson/document.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

extern std::map<std::string, ark::EntityScene*> gEntityScenes;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  EntityScene::EntityScene(
    const std::string& Entry,
    const std::string& SubEntry,
    const std::string& SceneName,
    const std::string& WindowName)
    : Scene{ eSceneTypeEntity, Entry, SubEntry, SceneName, WindowName }
  {
    Load();

    ModelsToActors();
  }

  EntityScene::~EntityScene()
  {
    Save();
  }

  void EntityScene::Load()
  {
    fs::path lvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mEntry / mSubEntry;
    fs::path datDir = lvlDir / fs::path{ mEntry + mSubEntry + ".dat" };

    fs::path mdFile = FsUtils::SearchFileByType(datDir, "MD");

    auto ddsFiles = FsUtils::SearchFilesByTypeRecursive(datDir, "DDS");

    auto models = MdSerializer::FromFile(mdFile);

    mModels.insert(mModels.end(), models.begin(), models.end());

    for (const auto& file : ddsFiles)
    {
      mTextures.emplace_back(TextureUtils::LoadDDS(file));
    }
  }

  void EntityScene::Save()
  {

  }

  void EntityScene::ModelsToActors()
  {
    for (const auto& [model, trans] : mModels)
    {
      Actor* modelActor = CreateActor<Actor>(model.Name, nullptr);

      Transform* transform = modelActor->GetTransform();

      transform->SetLocalPosition(R32V3{ trans.Position.x, trans.Position.y, trans.Position.z });
      transform->SetLocalRotation(glm::degrees(R32V3{ trans.Rotation.x, trans.Rotation.y, trans.Rotation.z } / 360.0F));
      transform->SetLocalScale(R32V3{ trans.Scale.x, trans.Scale.y, trans.Scale.z });

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
}