#include <string>
#include <filesystem>

#include <Common/Debug.h>
#include <Common/Types.h>
#include <Common/BlowFish.h>

#include <Common/Trees/ArchiveNode.h>

#include <Common/Utils/FileUtils.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

rapidjson::Document gConfig = {};
rapidjson::Document gWorld = {};

///////////////////////////////////////////////////////////
// Unpack Utilities
///////////////////////////////////////////////////////////

static void CreateDirIfNotExists(const fs::path& Dir)
{
  if (!fs::exists(Dir))
  {
    fs::create_directory(Dir);
  }
}

static const rapidjson::Value& GetLevelNameWithId(const rapidjson::Value& Levels, const std::string& Id)
{
  for (const auto& level : Levels.GetArray())
  {
    if (level["id"].GetString() == Id)
    {
      return level;
    }
  }

  return Levels;
}

static void ExtractArchiveRecursive(const fs::path& Dir, ark::ArchiveNode* Node)
{
  if (Node->IsArchive())
  {
    for (const auto& [type, node] : *Node)
    {
      ExtractArchiveRecursive(Dir, node);
    }
  }
  else
  {
    fs::path fileWithName = Dir;

    if (Node->GetName() == "")
    {
      fileWithName /= std::to_string(Node->GetCrc32());
    }
    else
    {
      fileWithName /= Node->GetName();
    }

    std::string fileWithNameAndExtension = fileWithName.string() + "." + Node->GetType();

    if (Node->GetSize())
    {
      if (fs::exists(fileWithNameAndExtension))
      {
        if (Node->GetSize() > fs::file_size(fileWithNameAndExtension))
        {
          ark::FileUtils::WriteBinary(fileWithNameAndExtension, Node->GetBytes());
        }
      }
      else
      {
        ark::FileUtils::WriteBinary(fileWithNameAndExtension, Node->GetBytes());
      }
    }
  }
}

///////////////////////////////////////////////////////////
// Entry Point
///////////////////////////////////////////////////////////

ark::I32 main()
{
  gConfig.Parse(ark::FileUtils::ReadText("Config.json").c_str());
  gWorld.Parse(ark::FileUtils::ReadText("World.json").c_str());

  if (!fs::exists(gConfig["sourceDir"].GetString()))
  {
    LOG("Please specify valid sourceDir\n");
    return 0;
  }

  CreateDirIfNotExists(gConfig["unpackDir"].GetString());
  CreateDirIfNotExists(gConfig["repackDir"].GetString());

  ark::BlowFish cypher = { gConfig["encryptionKey"].GetString() };

  LOG("\n");
  LOG("Unpacking please wait ...\n");

  for (auto& region : gWorld["regions"].GetArray())
  {
    std::string regionId = region["id"].GetString();

    LOG("\n");
    LOG("Extracting region %s\n", region["name"].GetString());

    fs::path sourceDir = fs::path{ gConfig["sourceDir"].GetString() };
    fs::path unpackDir = fs::path{ gConfig["unpackDir"].GetString() };

    CreateDirIfNotExists(unpackDir);
    CreateDirIfNotExists(unpackDir / regionId);

    for (const auto& file : fs::directory_iterator{ sourceDir / std::string{ std::string{ "data_pc/st" } + regionId } })
    {
      std::string levelId = file.path().stem().string().substr(2, 2);
      std::string levelName = GetLevelNameWithId(region["levels"], levelId)["name"].GetString();

      LOG("  Extracting level %s from %s\n", levelName.c_str(), file.path().filename().string().c_str());

      CreateDirIfNotExists(unpackDir / regionId / levelId);

      fs::path sourceFile = sourceDir / "data_pc" / std::string{ "st" + regionId } / file.path().filename();

      std::vector<ark::U8> bytes = ark::FileUtils::ReadBinary(sourceFile.string());

      cypher.Decrypt(bytes);

      ark::ArchiveNode archive = { bytes };

      ExtractArchiveRecursive(unpackDir / regionId / levelId, &archive);
    }
  }

  LOG("\n");
  LOG("Extraction finished successfully\n");

  return 0;
}