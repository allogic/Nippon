#include <Common/Debug.h>

#include <Common/Utils/ByteUtils.h>
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
        U32 currCrc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(file.path()));

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
        U32 currCrc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(file.path()));

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
    U32 failCount = 0;
    U32 passCount = 0;

    fs::path decryptDir = gConfig["decryptDir"].GetString();
    fs::path repackDir = gConfig["repackDir"].GetString();

    for (const auto& entry : fs::directory_iterator{ repackDir })
    {
      for (const auto& subEntry : fs::directory_iterator{ entry })
      {
        for (const auto& file : fs::directory_iterator{ subEntry })
        {
          fs::path decryptedFile = decryptDir / entry.path().stem() / subEntry.path().stem() / file.path().filename();
          fs::path repackedFile = repackDir / entry.path().stem() / subEntry.path().stem() / file.path().filename();

          std::vector<U8> decryptedBytes = FileUtils::ReadBinary(decryptedFile);
          std::vector<U8> repackedBytes = FileUtils::ReadBinary(repackedFile);

          U64 index = ByteUtils::Compare(decryptedBytes, repackedBytes);

          fs::path posixFile = StringUtils::PosixPath(file.path().string());

          if (index == 0)
          {
            passCount++;

            LOG("%s -> Passed\n", posixFile.string().c_str());
          }
          else
          {
            failCount++;

            LOG("%s -> Missmatch at 0x%llX\n", posixFile.string().c_str(), index);
          }
        }
      }
    }

    LOG("\n");
    LOG("Failed: %10d\n", failCount);
    LOG("Passed: %10d\n", passCount);
    LOG("\n");

    return failCount == 0;
  }

  bool Integrity::CompareEncryptedWithOriginal()
  {
    bool intact = true;

    return intact;
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

        U32 crc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(file.path()));

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

        U32 crc32 = IntegrityUtils::Crc32FromBytes(FileUtils::ReadBinary(file.path()));

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