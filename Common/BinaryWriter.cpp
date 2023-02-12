#include <Common/BinaryWriter.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void BinaryWriter::SeekRelative(I64 Value)
  {
    mPosition += Value;

    if (mPosition >= mBytes.size())
    {
      mBytes.resize(mPosition);
    }
  }

  void BinaryWriter::SeekAbsolute(I64 Value)
  {
    mPosition = Value;

    if (mPosition >= mBytes.size())
    {
      mBytes.resize(mPosition);
    }
  }

  void BinaryWriter::String(const std::string& Value, U64 Count)
  {
    if (mPosition >= mBytes.size())
    {
      mBytes.resize(mPosition + Count);
    }

    if (Count > 0)
    {
      std::memset(&mBytes[mPosition], 0, Count);
      std::memcpy(&mBytes[mPosition], &Value[0], Value.size());
    }

    mPosition += Count;
  }
}