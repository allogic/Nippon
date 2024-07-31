#include <Common/Utility/FileUtility.h>

namespace Nippon
{
	void FileUtility::ReadBinary(fs::path const& FilePath, std::vector<U8>& Bytes, U64 Size)
	{
		std::ifstream stream = std::ifstream{ FilePath, std::ios::ate | std::ios::binary };

		if (stream.is_open())
		{
			if (Size == 0)
			{
				Size = stream.tellg();
			}
			else
			{
				Size = std::min(Size, (U64)stream.tellg());
			}

			Bytes.resize(Size);

			stream.seekg(std::ios::beg);

			if (Size)
			{
				stream.read((char*)Bytes.data(), Size);
			}

			stream.close();
		}
	}

	void FileUtility::ReadText(fs::path const& FilePath, std::string& Text, U64 Size)
	{
		std::ifstream stream = std::ifstream{ FilePath, std::ios::ate };

		if (stream.is_open())
		{
			if (Size == 0)
			{
				Size = stream.tellg();
			}
			else
			{
				Size = std::min(Size, (U64)stream.tellg());
			}

			Text.resize(Size);

			stream.seekg(std::ios::beg);

			if (Size)
			{
				stream.read(Text.data(), Size);
			}

			stream.close();
		}
	}

	void FileUtility::WriteBinary(fs::path const& FilePath, std::vector<U8> const& Bytes, U64 Size)
	{
		std::ofstream stream = std::ofstream{ FilePath, std::ios::binary };

		if (stream.is_open())
		{
			if (Size == 0)
			{
				Size = Bytes.size();
			}
			else
			{
				Size = std::min(Size, (U64)Bytes.size());
			}

			if (Size)
			{
				stream.write((char const*)Bytes.data(), Size);
			}

			stream.close();
		}
	}

	void FileUtility::WriteBinary(fs::path const& FilePath, U8 const* Bytes, U64 Size)
	{
		std::ofstream stream = std::ofstream{ FilePath, std::ios::binary };

		if (stream.is_open())
		{
			stream.write((char const*)Bytes, Size);
			stream.close();
		}
	}

	void FileUtility::WriteText(fs::path const& FilePath, std::string const& Text, U64 Size)
	{
		std::ofstream stream = std::ofstream{ FilePath };

		if (stream.is_open())
		{
			if (Size == 0)
			{
				Size = Text.size();
			}
			else
			{
				Size = std::min(Size, (U64)Text.size());
			}

			if (Size)
			{
				stream.write(Text.data(), Size);
			}

			stream.close();
		}
	}

	void FileUtility::WriteText(fs::path const& FilePath, U8 const* Text, U64 Size)
	{
		std::ofstream stream = std::ofstream{ FilePath };

		if (stream.is_open())
		{
			stream.write((char const*)Text, Size);
			stream.close();
		}
	}

	void FileUtility::CreateDirIfNotExist(fs::path const& FilePath, bool DotIsDirectory)
	{
		fs::path partial = "";

		for (auto const& filePath : FilePath)
		{
			partial /= filePath;

			if (DotIsDirectory)
			{

			}
			else
			{
				if (filePath.string().contains('.'))
				{
					break;
				}
			}

			if (!fs::exists(partial))
			{
				fs::create_directory(partial);
			}
		}
	}

	std::vector<U64> FileUtility::SearchStringsInFile(std::vector<U8> const& Bytes, std::vector<U8> const& Pattern)
	{
		std::vector<U64> indices = {};

		for (U64 i = 0; i < Bytes.size(); i++)
		{
			bool found = true;

			for (U32 j = 0; j < Pattern.size(); j++)
			{
				if (Bytes[i + j] != Pattern[j])
				{
					found = false;

					break;
				}
			}

			if (found)
			{
				indices.emplace_back(i);
			}
		}

		return indices;
	}
}