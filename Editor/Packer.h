#pragma once

#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include <algorithm>

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
  class Packer
  {
  public:

    static void DecryptArchive(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
    static void EncryptArchive(const std::string& Entry, const std::string& SubEntry, const std::string& Name);

    static void UnpackArchive(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
    static void RepackArchive(const std::string& Entry, const std::string& SubEntry, const std::string& Name);
  };
}