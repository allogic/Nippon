#include <Common/Debug.h>

#include <Common/Utils/ByteUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  U64 ByteUtils::Compare(const std::vector<U8>& Lhs, const std::vector<U8>& Rhs)
  {
    U64 size = std::min(Lhs.size(), Rhs.size());

    for (U64 i = 0; i < size; i++)
    {
      if (Lhs[i] != Rhs[i])
      {
        return i;
      }
    }

    return 0;
  }

  std::vector<U64> ByteUtils::Search(const std::vector<U8>& Bytes, const std::vector<U8>& Pattern)
  {
    std::vector<U64> indices = {};

    for (U64 i = 0; i < Bytes.size(); i++)
    {
      bool found = true;

      for (U32 j = 0; j < Pattern.size(); j++)
      {
        if (Bytes[i + j] != Pattern[j])
        {
          found = false;

          break;
        }
      }

      if (found)
      {
        indices.emplace_back(i);
      }
    }

    return indices;
  }
}