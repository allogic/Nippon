#pragma once

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
  class Packer
  {
  public:

    static void Pack();
    static void UnPack();

  public:

    static void GenerateIntegrityMap();

  private:

    static void CreateDirIfNotExists(const fs::path& File);
  };
}