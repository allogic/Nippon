#pragma once

#include <string>
#include <cstring>
#include <filesystem>
#include <vector>
#include <set>
#include <map>
#include <fstream>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class ThumbnailContainer
	{
	public:

		friend class ThumbnailDatabase;

	public:

		enum FileType : U32
		{
			eFileTypeNone,
			eFileTypeLevel,
			eFileTypeEntity,
		};

	public:

		inline const auto& GetIdentifier() const { return mIdentifier; }
		inline const auto& GetType() const { return mType; }
		inline const auto& GetTexture() const { return mTexture; }

	private:

		U32 mIdentifier = 0;

		FileType mType = eFileTypeNone;

		U32 mTexture = 0;
	};

	class ThumbnailDatabase
	{
	public:

		static void Create();
		static void Generate();
		static void Destroy();

	public:

		static const ThumbnailContainer& GetThumbnailContainerByIdentifier(U32 Identifier);

	private:

		static void LoadThumbnails();
	};
}
