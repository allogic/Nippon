#pragma once

#include <string>

#include <Common/Types.h>

namespace ark
{
	class GenericTexture
	{
	public:

		enum TextureType
		{
			eTextureTypeNone,
			eTextureTypeScr,
			eTextureTypeMd,
		};

	public:

		inline const auto& GetType() const { return mType; }
		inline const auto& GetName() const { return mName; }
		inline const auto& GetTexture() const { return mTexture; }

	public:

		inline void SetType(TextureType Value) { mType = Value; }
		inline void SetName(const std::string& Value) { mName = Value; }
		inline void SetTexture(U32 Value) { mTexture = Value; }

	private:

		TextureType mType = eTextureTypeNone;

		std::string mName = "";

		U32 mTexture = 0;
	};
}
