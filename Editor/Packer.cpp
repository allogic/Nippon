#include <Common/Debug.h>
#include <Common/BlowFish.h>

#include <Common/Utils/DirUtils.h>
#include <Common/Utils/FileUtils.h>
#include <Common/Utils/JsonUtils.h>
#include <Common/Utils/StringUtils.h>

#include <Common/Recursion/ArchiveExtractionNode.h>
#include <Common/Recursion/ArchiveCompressionNode.h>

#include <Editor/Packer.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gArchive;
extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  const std::set<std::string> sSupportedArchiveExtensions =
  {
    ".dat",
    ".bin",
  };

  void Packer::DecryptArchive(const std::string& Entry, const std::string& SubEntry)
  {
    fs::path gameDir = gConfig["gameDir"].GetString();
    fs::path dataDir = gameDir / "data_pc";
    fs::path decryptDir = gConfig["decryptDir"].GetString();

    DirUtils::CreateIfNotExists(decryptDir);
    DirUtils::CreateIfNotExists(decryptDir / Entry);
    DirUtils::CreateIfNotExists(decryptDir / Entry / SubEntry);

    for (const auto& file : fs::directory_iterator{ dataDir / Entry })
    {
      std::string selection = StringUtils::SelectExpr(file.path().stem().string(), "??XX");
      
      if (selection == SubEntry)
      {
        fs::path srcFile = dataDir / Entry / file.path().filename().string();
        fs::path dstFile = decryptDir / Entry / SubEntry / file.path().filename().string();

        if (fs::exists(srcFile))
        {
          std::vector<U8> bytes = FileUtils::ReadBinary(srcFile.string());

          BlowFish cipher{ gConfig["encryptionKey"].GetString() };

          cipher.Decrypt(bytes);

          FileUtils::WriteBinary(dstFile.string(), bytes);

          std::string posixSrcFile = StringUtils::PosixPath(srcFile.string());
          std::string posixDstFile = StringUtils::PosixPath(dstFile.string());

          LOG("%s -> %s\n", posixSrcFile.c_str(), posixDstFile.c_str());
        }
      }
    }
  }

  void Packer::EncryptArchive(const std::string& Entry, const std::string& SubEntry)
  {
    fs::path repackDir = gConfig["repackDir"].GetString();
    fs::path encryptDir = gConfig["encryptDir"].GetString();

    DirUtils::CreateIfNotExists(encryptDir);
    DirUtils::CreateIfNotExists(encryptDir / Entry);
    DirUtils::CreateIfNotExists(encryptDir / Entry / SubEntry);

    for (const auto& file : fs::directory_iterator{ repackDir / Entry })
    {
      std::string selection = StringUtils::SelectExpr(file.path().stem().string(), "??XX");

      if (selection == SubEntry)
      {
        fs::path srcFile = repackDir / Entry / file.path().filename().string();
        fs::path dstFile = encryptDir / Entry / file.path().filename().string();

        if (fs::exists(srcFile))
        {
          std::vector<U8> bytes = FileUtils::ReadBinary(srcFile.string());

          BlowFish cipher{ gConfig["encryptionKey"].GetString() };

          cipher.Encrypt(bytes);

          FileUtils::WriteBinary(dstFile.string(), bytes);

          std::string posixSrcFile = StringUtils::PosixPath(srcFile.string());
          std::string posixDstFile = StringUtils::PosixPath(dstFile.string());

          LOG("%s -> %s\n", posixSrcFile.c_str(), posixDstFile.c_str());
        }
      }
    }
  }

  void Packer::UnpackArchive(const std::string& Entry, const std::string& SubEntry)
  {
    fs::path decryptDir = gConfig["decryptDir"].GetString();
    fs::path unpackDir = gConfig["unpackDir"].GetString();

    DirUtils::CreateIfNotExists(unpackDir);
    DirUtils::CreateIfNotExists(unpackDir / Entry);
    DirUtils::CreateIfNotExists(unpackDir / Entry / SubEntry);

    if (fs::exists(decryptDir / Entry / SubEntry))
    {
      for (const auto& file : fs::directory_iterator{ decryptDir / Entry / SubEntry })
      {
        std::string archiveName = file.path().filename().string();
        std::string archiveExt = file.path().extension().string();

        if (sSupportedArchiveExtensions.contains(archiveExt))
        {
          std::vector<U8> bytes = FileUtils::ReadBinary(file.path().string());

          DirUtils::CreateIfNotExists(unpackDir / Entry / SubEntry / archiveName);

          ArchiveExtractionNode extractor{ bytes, nullptr, 0, 0, 0, "", "" };
          
          extractor.ExtractRecursive(2, unpackDir / Entry / SubEntry / archiveName);

          std::string posixSrcFile = StringUtils::PosixPath(file.path().string());
          std::string posixDstFile = StringUtils::PosixPath((unpackDir / Entry / SubEntry / archiveName).string());

          LOG("%s -> %s\n", posixSrcFile.c_str(), posixDstFile.c_str());
        }
      }
    }
  }

  void Packer::RepackArchive(const std::string& Entry, const std::string& SubEntry)
  {
    fs::path unpackDir = gConfig["unpackDir"].GetString();
    fs::path repackDir = gConfig["repackDir"].GetString();

    DirUtils::CreateIfNotExists(repackDir);
    DirUtils::CreateIfNotExists(repackDir / Entry);

    for (const auto& file : fs::directory_iterator{ unpackDir / Entry / SubEntry })
    {
      std::string fileName = file.path().filename().string();

      if (fs::exists(unpackDir / Entry / SubEntry / fileName))
      {
        ArchiveCompressionNode compressor{ unpackDir / Entry / SubEntry / fileName, nullptr };

        compressor.CompressRecursive(2);

        FileUtils::WriteBinary((repackDir / Entry / fileName).string(), compressor.GetBytes());

        std::string posixSrcFile = StringUtils::PosixPath((unpackDir / Entry / SubEntry).string());
        std::string posixDstFile = StringUtils::PosixPath((repackDir / Entry / SubEntry).string());

        LOG("%s -> %s\n", posixSrcFile.c_str(), posixDstFile.c_str());
      }
    }
  }
}