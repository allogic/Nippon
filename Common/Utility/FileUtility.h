#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

namespace Nippon
{
	class FileUtility
	{
	public:

		static void ReadBinary(fs::path const& FilePath, std::vector<U8>& Bytes, U64 Size = 0);
		static void ReadText(fs::path const& FilePath, std::string& Text, U64 Size = 0);

		static void WriteBinary(fs::path const& FilePath, std::vector<U8> const& Bytes, U64 Size = 0);
		static void WriteBinary(fs::path const& FilePath, U8 const* Bytes, U64 Size);

		static void WriteText(fs::path const& FilePath, std::string const& Text, U64 Size = 0);
		static void WriteText(fs::path const& FilePath, U8 const* Text, U64 Size);

		static void CreateDirIfNotExist(fs::path const& FilePath, bool DotIsDirectory = false);

		static std::vector<U64> SearchStringsInFile(std::vector<U8> const& Bytes, std::vector<U8> const& Pattern);
	};
}