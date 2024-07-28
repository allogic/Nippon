#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#define OKAMI_CIPHER_KEY "YaKiNiKuM2rrVrPJpGMkfe3EK4RbpbHw"

namespace Nippon
{
	class BlowFish
	{
	public:

		static void Create(std::string const& Key);

	public:

		static void Encrypt(U32* XL, U32* XR);
		static void Decrypt(U32* XL, U32* XR);

		static void Encrypt(std::vector<U8>& Bytes);
		static void Decrypt(std::vector<U8>& Bytes);

	private:

		static U32 Feistel(U32 X);
	};
}