#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Header.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  struct ScrDivision
  {
    MdHeader Header;
    std::vector<ScrVertex> Vertices;
    std::vector<ScrTextureMap> TextureMaps;
    std::vector<ScrUv> TextureUvs;
    std::vector<ScrColorWeight> ColorWeights;
  };

  struct MdDivision
  {
    MdHeader Header;
    std::vector<MdVertex> Vertices;
    std::vector<MdTextureMap> TextureMaps;
    std::vector<MdUv> TextureUvs;
    std::vector<MdColorWeight> ColorWeights;
  };

  struct ScrModel
  {
    MdbHeader Header;
    std::string Name;
    std::vector<ScrDivision> Divisions;
  };

  struct MdModel
  {
    MdbHeader Header;
    std::string Name;
    std::vector<MdDivision> Divisions;
  };
}