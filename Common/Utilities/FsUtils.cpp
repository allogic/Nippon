#include <Common/Utilities/FsUtils.h>

namespace ark
{
	std::vector<U8> FsUtils::ReadBinary(const fs::path& File, U64 Size)
	{
		std::vector<U8> bytes = {};
		std::ifstream stream = std::ifstream{ File, std::ios::ate | std::ios::binary };

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

			bytes.resize(Size);

			stream.seekg(std::ios::beg);

			if (Size)
			{
				stream.read((char*)&bytes[0], Size);
			}

			stream.close();
		}

		return bytes;
	}

	std::string FsUtils::ReadText(const fs::path& File, U64 Size)
	{
		std::string text = "";
		std::ifstream stream = std::ifstream{ File, std::ios::ate };

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

			text.resize(Size);

			stream.seekg(std::ios::beg);

			if (Size)
			{
				stream.read(&text[0], Size);
			}

			stream.close();
		}

		return text;
	}

	void FsUtils::WriteBinary(const fs::path& File, const std::vector<U8>& Bytes, U64 Size)
	{
		std::ofstream stream = std::ofstream{ File, std::ios::binary };

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
				stream.write((char*)&Bytes[0], Size);
			}

			stream.close();
		}
	}

	void FsUtils::WriteBinary(const fs::path& File, U8* Bytes, U64 Size)
	{
		std::ofstream stream = std::ofstream{ File, std::ios::binary };

		if (stream.is_open())
		{
			stream.write((char*)Bytes, Size);
			stream.close();
		}
	}

	void FsUtils::WriteText(const fs::path& File, const std::string& Text, U64 Size)
	{
		std::ofstream stream = std::ofstream{ File };

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
				stream.write(&Text[0], Size);
			}

			stream.close();
		}
	}

	void FsUtils::WriteText(const fs::path& File, U8* Text, U64 Size)
	{
		std::ofstream stream = std::ofstream{ File };

		if (stream.is_open())
		{
			stream.write((char*)Text, Size);
			stream.close();
		}
	}

	void FsUtils::CreateDirIfNotExist(const fs::path& File, bool DotIsDirectory)
	{
		fs::path partial = "";

		for (const auto& file : File)
		{
			partial /= file;

			if (DotIsDirectory)
			{

			}
			else
			{
				if (file.string().contains('.'))
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

	std::vector<U64> FsUtils::SearchStringsInFile(const std::vector<U8>& Bytes, const std::vector<U8>& Pattern)
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
