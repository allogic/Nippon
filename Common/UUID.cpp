#include <Common/UUID.h>

namespace ark
{
	std::string UUID::Generate()
	{
		auto randomDevice = std::random_device{};
		auto randomEngine = std::default_random_engine{ randomDevice() };
		auto integerDistribution = std::uniform_int_distribution<U32>{ 0, 256 };

		U8 bytes[16] = {};

		for (U32 i = 0; i < 16; i++)
		{
			bytes[i] = integerDistribution(randomEngine);
		}

		bytes[6] = (bytes[6] & 0x0F) | 0x40; // Version 4
		bytes[8] = (bytes[8] & 0x3F) | 0x80; // Variant bits

		I8 buffer[36];

		std::snprintf(buffer, sizeof(buffer),
			"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7],
			bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);

		return buffer;
	}
}
