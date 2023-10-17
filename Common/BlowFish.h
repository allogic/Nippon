#pragma once

#include <vector>
#include <string>

#include <Common/Types.h>

namespace ark
{
	class BlowFish
	{
	public:

		BlowFish(const std::string& Key);

	public:

		void Encrypt(U32* XL, U32* XR);
		void Decrypt(U32* XL, U32* XR);

		void Encrypt(std::vector<U8>& Bytes);
		void Decrypt(std::vector<U8>& Bytes);

	private:

		U32 Feistel(U32 X);

		U32 mP[16 + 2] = {};
		U32 mS[4][256] = {};
	};
}
