#include <Common/Debug.h>
#include <Common/BlowFish.h>
#include <Common/Crc32.h>

#include <Common/Trees/ArchiveNode.h>

#include <Common/Utils/DirUtils.h>
#include <Common/Utils/FileUtils.h>
#include <Common/Utils/JsonUtils.h>
#include <Common/Utils/StringUtils.h>

#include <Editor/Packer.h>

#include <Vendor/rapidjson/document.h>
#include "Vendor/rapidjson/prettywriter.h"
#include "Vendor/rapidjson/stringbuffer.h"

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;
extern rj::Document gPacker;
extern rj::Document gIntegrity;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void Packer::Unpack()
  {
    LOG("Unpacking, please wait...\n");

    BlowFish cypher = { gPacker["encryptionKey"].GetString() };

    fs::path gameDir = gConfig["gameDir"].GetString();
    fs::path dataDir = gameDir / "data_pc";
    fs::path unpackDir = gConfig["unpackDir"].GetString();

    DirUtils::CreateIfNotExists(unpackDir);

    for (const auto& source : gPacker["sources"].GetArray())
    {
      std::string targetDir = source["targetDir"].GetString();

      DirUtils::CreateIfNotExists(unpackDir / targetDir);

      std::set<std::string> extensions = JsonUtils::ToStringSet(source["extensions"].GetArray());

      for (const auto& file : fs::directory_iterator{ dataDir / targetDir })
      {
        if (extensions.contains(file.path().extension().string()))
        {
          std::string dirName = StringUtils::SelectExpr(file.path().stem().string(), source["selectExpr"].GetString());
          std::vector<U8> bytes = FileUtils::ReadBinary(file.path().string());

          cypher.Decrypt(bytes);
        
          DirUtils::CreateIfNotExists(unpackDir / targetDir / dirName);

          ArchiveNode{ bytes }.ExtractRecursive(unpackDir / targetDir / dirName);

          std::string posixFile = StringUtils::PosixPath(file.path().string());
          std::string posixDir = StringUtils::PosixPath(dataDir.string());

          LOG("  Unpacking %s\n", StringUtils::CutFront(posixFile, posixDir.size()).c_str());
        }
      }
    }

    LOG("Unpacking finished successfully!\n");
    LOG("\n");
  }

  void Packer::Repack()
  {
    LOG("Repacking, please wait...\n");

    LOG("Repacking finished successfully!\n");
    LOG("\n");
  }

  void Packer::CheckIntegrity()
  {
    LOG("Checking integrity, please wait...\n");

    U32 success = 1;
    fs::path gameDir = gConfig["gameDir"].GetString();
    fs::path dataDir = gameDir / "data_pc";

    rj::Document integrity = {};

    integrity.Parse(FileUtils::ReadText("Integrity.json").c_str());

    for (const auto& file : fs::recursive_directory_iterator{ dataDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string posixFile = StringUtils::PosixPath(file.path().string());
        std::string posixDir = StringUtils::PosixPath(dataDir.string());
        std::string keyValue = StringUtils::CutFront(posixFile, posixDir.size());

        U32 origCrc32 = integrity[keyValue.c_str()].GetUint();
        U32 currCrc32 = Crc32::FromBytes(FileUtils::ReadBinary(posixFile));

        if (origCrc32 != currCrc32)
        {
          success = 0;
        }

        LOG("  [%s] %s\n", (origCrc32 == currCrc32) ? "Ok" : "Failed", keyValue.c_str());
      }
    }

    LOG("Integrity check %s!\n", (success) ? "successful" : "unsuccessful");
    LOG("\n");
  }

  void Packer::GenerateIntegrityMap()
  {
    LOG("Generating integrity, please wait...\n");

    fs::path gameDir = gConfig["gameDir"].GetString();
    fs::path dataDir = gameDir / "data_pc";

    rj::Document document;
    rj::Value integrities = rj::Value{ rj::kObjectType };
    rj::StringBuffer buffer;
    rj::PrettyWriter<rj::StringBuffer> writer = rj::PrettyWriter<rj::StringBuffer>{ buffer };

    for (const auto& file : fs::recursive_directory_iterator{ dataDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string posixFile = StringUtils::PosixPath(file.path().string());
        std::string posixDir = StringUtils::PosixPath(dataDir.string());
        std::string keyValue = StringUtils::CutFront(posixFile, posixDir.size());

        U32 crc32 = Crc32::FromBytes(FileUtils::ReadBinary(posixFile));

        integrities.AddMember(
          rj::Value{ rj::kStringType }.SetString(keyValue.c_str(), document.GetAllocator()),
          rj::Value{ rj::kNumberType }.SetUint(crc32),
          document.GetAllocator());

        LOG("  0x%08X %s\n", crc32, keyValue.c_str());
      }
    }

    integrities.Accept(writer);

    FileUtils::WriteText("Integrity.json", buffer.GetString());

    LOG("Integrity generated successfully!\n");
    LOG("\n");
  }
}