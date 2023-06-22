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
}