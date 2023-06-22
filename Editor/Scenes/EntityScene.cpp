#include <Common/Utils/FsUtils.h>
#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Scenes/EntityScene.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Interface/Outline.h>

#include <Vendor/rapidjson/document.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void EntityScene::Create(const std::string& Entry, const std::string& SubEntry, const std::string& Name, std::vector<EntityScene*>& Scenes)
  {
    Scenes.emplace_back(new EntityScene{ Entry, SubEntry, Name });
  }

  EntityScene::EntityScene(const std::string& Entry, const std::string& SubEntry, const std::string& Name)
    : Scene{ Entry, SubEntry, Name }
  {
    Load();
  }

  EntityScene::~EntityScene()
  {
    Save();
  }

  void EntityScene::Load()
  {
    //fs::path lvlDir = fs::path{ gConfig["unpackDir"].GetString() } / mRegion / mLevel;
    //fs::path datDir = lvlDir / fs::path{ "r" + mMapId + mLevel + ".dat" };
  }

  void EntityScene::Save()
  {

  }
}