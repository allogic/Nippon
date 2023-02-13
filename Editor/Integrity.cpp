#include <Common/Debug.h>

#include <Common/Utils/FileUtils.h>
#include <Common/Utils/IntegrityUtils.h>
#include <Common/Utils/StringUtils.h>

#include <Editor/Integrity.h>

#include <Vendor/rapidjson/document.h>
#include "Vendor/rapidjson/prettywriter.h"
#include "Vendor/rapidjson/stringbuffer.h"

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  bool Integrity::CheckEncrypted()
  {
    bool intact = true;

    fs::path encryptDir = gConfig["encryptDir"].GetString();

    rj::Document integrity = {};

    integrity.Parse(FileUtils::ReadText("EncryptedIntegrity.json").c_str());

    for (const auto& file : fs::recursive_directory_iterator{ encryptDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string posixFile = StringUtils::PosixPath(file.path().string());
        std::string posixDir = StringUtils::PosixPath(encryptDir.string());
        std::string keyValue = StringUtils::CutFront(posixFile, posixDir.size());

        U32 origCrc32 = integrity[keyValue.c_str()].GetUint();
        U32 currCrc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(posixFile));

        if (origCrc32 != currCrc32)
        {
          intact = false;
        }

        LOG("%s -> %s\n", keyValue.c_str(), (origCrc32 == currCrc32) ? "Passed" : "Failed");
      }
    }

    return intact;
  }

  bool Integrity::CheckDecrypted()
  {
    bool intact = true;

    fs::path decryptDir = gConfig["decryptDir"].GetString();

    rj::Document integrity = {};

    integrity.Parse(FileUtils::ReadText("DecryptedIntegrity.json").c_str());

    for (const auto& file : fs::recursive_directory_iterator{ decryptDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string posixFile = StringUtils::PosixPath(file.path().string());
        std::string posixDir = StringUtils::PosixPath(decryptDir.string());
        std::string keyValue = StringUtils::CutFront(posixFile, posixDir.size());

        U32 origCrc32 = integrity[keyValue.c_str()].GetUint();
        U32 currCrc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(posixFile));

        if (origCrc32 != currCrc32)
        {
          intact = false;
        }

        LOG("%s -> %s\n", keyValue.c_str(), (origCrc32 == currCrc32) ? "Passed" : "Failed");
      }
    }

    return intact;
  }

  bool Integrity::CompareRepackedWithDecrypted()
  {
    return false;
  }

  bool Integrity::CompareEncryptedWithOriginal()
  {
    return false;
  }

  void Integrity::GenerateEncryptedMap()
  {
    fs::path encryptDir = gConfig["encryptDir"].GetString();

    rj::Document document;
    rj::Value integrities = rj::Value{ rj::kObjectType };
    rj::StringBuffer buffer;
    rj::PrettyWriter<rj::StringBuffer> writer = rj::PrettyWriter<rj::StringBuffer>{ buffer };

    for (const auto& file : fs::recursive_directory_iterator{ encryptDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string posixFile = StringUtils::PosixPath(file.path().string());
        std::string posixDir = StringUtils::PosixPath(encryptDir.string());
        std::string keyValue = StringUtils::CutFront(posixFile, posixDir.size());

        U32 crc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(posixFile));

        integrities.AddMember(
          rj::Value{ rj::kStringType }.SetString(keyValue.c_str(), document.GetAllocator()),
          rj::Value{ rj::kNumberType }.SetUint(crc32),
          document.GetAllocator());

        LOG("0x%08X %s\n", crc32, keyValue.c_str());
      }
    }

    integrities.Accept(writer);

    FileUtils::WriteText("EncryptedIntegrity.json", buffer.GetString());
  }

  void Integrity::GenerateDecryptedMap()
  {
    fs::path decryptDir = gConfig["decryptDir"].GetString();

    rj::Document document;
    rj::Value integrities = rj::Value{ rj::kObjectType };
    rj::StringBuffer buffer;
    rj::PrettyWriter<rj::StringBuffer> writer = rj::PrettyWriter<rj::StringBuffer>{ buffer };

    for (const auto& file : fs::recursive_directory_iterator{ decryptDir })
    {
      if (fs::is_regular_file(file))
      {
        std::string posixFile = StringUtils::PosixPath(file.path().string());
        std::string posixDir = StringUtils::PosixPath(decryptDir.string());
        std::string keyValue = StringUtils::CutFront(posixFile, posixDir.size());

        U32 crc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(posixFile));

        integrities.AddMember(
          rj::Value{ rj::kStringType }.SetString(keyValue.c_str(), document.GetAllocator()),
          rj::Value{ rj::kNumberType }.SetUint(crc32),
          document.GetAllocator());

        LOG("0x%08X %s\n", crc32, keyValue.c_str());
      }
    }

    integrities.Accept(writer);

    FileUtils::WriteText("DecryptedIntegrity.json", buffer.GetString());
  }
}