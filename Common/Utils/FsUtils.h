#pragma once

#include <cstdlib>
#include <string>
#include <cstring>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class FsUtils
	{
	public:

		static std::vector<U8> ReadBinary(const fs::path& File, U64 Size = 0);
		static std::string ReadText(const fs::path& File, U64 Size = 0);

		static void WriteBinary(const fs::path& File, const std::vector<U8>& Bytes, U64 Size = 0);
		static void WriteText(const fs::path& File, const std::string& Text, U64 Size = 0);

		static void CreateIfNotExists(const fs::path& File);

		static void FromArchiveName(const std::string& String, U16& Index, std::string& Name, std::string& Type);
		static std::string ToArchiveName(U16 Index, const std::string& Name, const std::string& Type);

		static fs::path SearchFileByType(const fs::path& File, const std::string& Type);
		static std::vector<fs::path> SearchFilesByTypeRecursive(const fs::path& File, const std::string& Type);
	};
}