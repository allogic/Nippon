#include <Editor/Converter/ElementConverter.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::vector<U32> ElementConverter::ToElementBuffer(const std::vector<ScrVertex>& Vertices)
  {
    std::vector<U32> elements = {};

    U16 vertexCount = (U16)Vertices.size();

    for (U16 i = 2; i < vertexCount; i++)
    {
      if (Vertices[i].Connection == 0x8000)
      {
        continue;
      }

      if (Vertices[i].Connection)
      {
        elements.emplace_back(i - 2);
        elements.emplace_back(i - 1);
        elements.emplace_back(i - 0);
      }
      else
      {
        elements.emplace_back(i - 1);
        elements.emplace_back(i - 2);
        elements.emplace_back(i - 0);
      }
    }

    return elements;
  }

  std::vector<U32> ElementConverter::ToElementBuffer(const std::vector<MdVertex>& Vertices)
  {
    std::vector<U32> elements = {};

    U16 vertexCount = (U16)Vertices.size();

    for (U16 i = 1; i < vertexCount; i++)
    {
      if (Vertices[i].Connection == 0x8000)
      {
        continue;
      }

      if (Vertices[i].Connection)
      {
        elements.emplace_back(i - 2);
        elements.emplace_back(i - 1);
        elements.emplace_back(i - 0);
      }
      else
      {
        elements.emplace_back(i - 1);
        elements.emplace_back(i - 2);
        elements.emplace_back(i - 0);
      }
    }

    return elements;
  }
}