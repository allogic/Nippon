#pragma once

#include <Standard.h>
#include <Types.h>

namespace Nippon
{
	class FileUtility
	{
	public:

		static std::vector<U8> ReadBinary(fs::path const& FilePath, U64 Size = 0);
		static std::string ReadText(fs::path const& FilePath, U64 Size = 0);

		static void WriteBinary(fs::path const& FilePath, std::vector<U8> const& Bytes, U64 Size = 0);
		static void WriteBinary(fs::path const& FilePath, U8 const* Bytes, U64 Size);

		static void WriteText(fs::path const& FilePath, std::string const& Text, U64 Size = 0);
		static void WriteText(fs::path const& FilePath, U8 const* Text, U64 Size);

		static void CreateDirIfNotExist(fs::path const& FilePath, bool DotIsDirectory = false);

		static std::vector<U64> SearchStringsInFile(std::vector<U8> const& Bytes, std::vector<U8> const& Pattern);
	};
}