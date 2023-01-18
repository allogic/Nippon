#include <Common/Utils/StringUtils.h>

#include <Vendor/CRC/crc32.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  std::string StringUtils::RemoveNulls(const std::string& String)
  {
    std::stringstream stream;

    for (U32 i = 0; i < String.size(); i++)
    {
      if (String[i] != 0)
      {
        stream << String[i];
      }
    }

    return stream.str();
  }

  U32 StringUtils::Crc32(const std::string& String, U64 DefaultChunkSize)
  {
    U32 crc = 0;
    U64 bytesProcessed = 0;
    U64 bytesLeft;
    U64 numBytes = String.size();
    U64 chunkSize;

    while (bytesProcessed < numBytes)
    {
      bytesLeft = numBytes - bytesProcessed;
      chunkSize = (DefaultChunkSize < bytesLeft) ? DefaultChunkSize : bytesLeft;

      crc = crc32_fast(&String[bytesProcessed], chunkSize, crc);

      bytesProcessed += chunkSize;
    }

    return crc;
  }

  U32 StringUtils::Crc32(const std::vector<U8>& Bytes, U64 DefaultChunkSize)
  {
    U32 crc = 0;
    U64 bytesProcessed = 0;
    U64 bytesLeft;
    U64 numBytes = Bytes.size();
    U64 chunkSize;

    while (bytesProcessed < numBytes)
    {
      bytesLeft = numBytes - bytesProcessed;
      chunkSize = (DefaultChunkSize < bytesLeft) ? DefaultChunkSize : bytesLeft;

      crc = crc32_fast(&Bytes[bytesProcessed], chunkSize, crc);

      bytesProcessed += chunkSize;
    }

    return crc;
  }
}