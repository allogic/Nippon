#include <Common/Utils/IntegrityUtils.h>

#include <Vendor/CRC/crc32.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	U32 IntegrityUtils::Crc32FromString(const std::string& String, U64 DefaultChunkSize)
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

	U32 IntegrityUtils::Crc32FromBytes(const std::vector<U8>& Bytes, U64 DefaultChunkSize)
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