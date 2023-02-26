#pragma once

#include <string>
#include <filesystem>

#include <Common/Types.h>

#include <Vendor/rapidjson/rapidjson.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;
namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Integrity
  {
  public:

    static bool CheckEncrypted();
    static bool CheckDecrypted();

    static void CompareRepackedWithDecrypted(const std::string& Entry, const std::string& SubEntry);
    static void CompareEncryptedWithOriginal(const std::string& Entry, const std::string& SubEntry);

    static void GenerateEncryptedMap();
    static void GenerateDecryptedMap();
  };
}