#pragma once

#include <cstdlib>
#include <string>
#include <cstring>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class FsUtils
	{
	public:

		static std::vector<U8> ReadBinary(const fs::path& File, U64 Size = 0);
		static std::string ReadText(const fs::path& File, U64 Size = 0);

		static void WriteBinary(const fs::path& File, const std::vector<U8>& Bytes, U64 Size = 0);
		static void WriteText(const fs::path& File, const std::string& Text, U64 Size = 0);

		static void CreateIfNotExist(const fs::path& File, bool DotIsDirectory = false);

		static std::vector<U64> SearchBytesInFile(const std::vector<U8>& Bytes, const std::vector<U8>& Pattern);
	};
}
