#include <Common/Debug.h>
#include <Common/BlowFish.h>

#include <Common/Utils/IntegrityUtils.h>
#include <Common/Utils/FileUtils.h>
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
  void Packer::Pack()
  {

  }

  void Packer::UnPack()
  {
    for (const auto& source : gPacker["sources"].GetArray())
    {

    }
  }

  void Packer::GenerateIntegrityMap()
  {
    fs::path sourceDir = gConfig["sourceDir"].GetString();
    fs::path dataDir = sourceDir / "data_pc";

    rj::Document document;
    rj::Value integrities = rj::Value{ rj::kObjectType };
    rj::StringBuffer buffer;
    rj::PrettyWriter<rj::StringBuffer> writer = rj::PrettyWriter<rj::StringBuffer>{ buffer };

    BlowFish cypher = { gPacker["encryptionKey"].GetString() };

    LOG("Generating integrity map, please wait...\n");

    for (const auto& file : fs::recursive_directory_iterator{ dataDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string canonicalFile = StringUtils::NormalizePath(file.path().string());
        std::string canonicalData = StringUtils::NormalizePath(dataDir.string());
        std::string keyValue = StringUtils::CutFront(canonicalFile, canonicalData.size());

        std::vector<U8> bytes = FileUtils::ReadBinary(canonicalFile);
        U32 sizeBytes = (U32)bytes.size();
        U32 crc32Encrypted = IntegrityUtils::Crc32(bytes);
        cypher.Decrypt(bytes);
        U32 crc32Decrypted = IntegrityUtils::Crc32(bytes);

        rj::Value integrity = rj::Value{ rj::kObjectType };
        integrity.AddMember("sizeBytes", rj::Value{ rj::kNumberType }.SetUint(sizeBytes), document.GetAllocator());
        integrity.AddMember("crc32Encrypted", rj::Value{ rj::kNumberType }.SetUint(crc32Encrypted), document.GetAllocator());
        integrity.AddMember("crc32Decrypted", rj::Value{ rj::kNumberType }.SetUint(crc32Decrypted), document.GetAllocator());

        integrities.AddMember(
          rj::Value{ rj::kStringType }.SetString(keyValue.c_str(), document.GetAllocator()),
          integrity,
          document.GetAllocator());

        LOG("  %10u %08X %08X %s\n", sizeBytes, crc32Encrypted, crc32Decrypted, keyValue.c_str());
      }
    }

    LOG("Integrity map successfully generated!\n");
    LOG("\n");

    integrities.Accept(writer);

    FileUtils::WriteText("Integrity.json", buffer.GetString());
  }

  void Packer::CreateDirIfNotExists(const fs::path& File)
  {
    if (!fs::exists(File))
    {
      fs::create_directory(File);
    }
  }
}