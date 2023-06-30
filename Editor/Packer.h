#pragma once

#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include <algorithm>

#include <Common/Types.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

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
  class Packer
  {
  public:

    static void DecryptArchive(const std::string& Entry, const std::string& SubEntry, rj::Value& Value);
    static void EncryptArchive(const std::string& Entry, const std::string& SubEntry, rj::Value& Value);

    static void UnpackArchive(const std::string& Entry, const std::string& SubEntry, rj::Value& Value);
    static void RepackArchive(const std::string& Entry, const std::string& SubEntry, rj::Value& Value);
  };
}