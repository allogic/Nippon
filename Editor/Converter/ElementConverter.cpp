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

    if (vertexCount >= 3)
    {
      for (U16 i = 2; i < vertexCount; i++)
      {
        if (Vertices[i].Connection == 0x8000)
        {
          continue;
        }
        else
        {
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
      }
    }

    return elements;
  }
}