#pragma once

#include <string>
#include <vector>
#include <utility>
#include <filesystem>

#include <Common/Types.h>

#include <Editor/Header.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  struct Division
  {
    MdHeader Header;
    std::vector<ScrVertex> Vertices;
    std::vector<U16V2> TextureMaps;
    std::vector<U16V2> TextureUvs;
    std::vector<U32> ColorWeights;
  };

  struct Model
  {
    MdbHeader Header;
    std::string Name;
    std::vector<Division> Divisions;
  };

  class ScrSerializer
  {
  public:

    static std::vector<std::pair<Model, ScrTransform>> FromFile(const fs::path& File);
    static void ToFile(const fs::path& File, const std::vector<std::pair<Model, ScrTransform>>& Objects);
  };
}