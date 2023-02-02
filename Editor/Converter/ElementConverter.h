#pragma once

#include <Common/Types.h>

#include <Editor/Serializer/ModelSerializer.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class ElementConverter
  {
  public:

    static std::vector<U32> ToElementBuffer(const std::vector<ScrVertex>& Vertices);
  };
}