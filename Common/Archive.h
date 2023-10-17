#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <filesystem>

#include <Common/Types.h>
#include <Common/BinaryReader.h>

namespace ark
{
	namespace fs = std::filesystem;

	struct ArchiveEntry
	{
		U32 Offset;
		U32 Size;
		std::string Type;
		std::string Name;
	};

	class Archive
	{
	public:

		Archive(Archive* Parent);
		virtual ~Archive();

	public:

		void LoadRecursive(const std::vector<U8>& Bytes, U16 Index, U32 Offset, U32 Size, const std::string& Type, const std::string& Name, bool IsDirectory);
		void SaveRecursive();

	public:

		inline const auto& GetName() const { return mName; }
		inline const auto& GetType() const { return mType; }
		inline const auto& GetOffset() const { return mOffset; }
		inline const auto& GetSize() const { return mSize; }

		inline const auto GetBytesWithHeader() const { return mBinaryReader.Bytes(mSize, 0); }
		inline const auto GetBytesWithoutHeader() const { return mBinaryReader.Bytes(mSize - 32, 32); }

	public:

		void DumpToDiskRecursive(fs::path File, Archive* Node = nullptr);
		void DumpTableOfContent(U32 Offset, U32 Indent, U32 Increment, Archive* Node = nullptr);
		void FindNodeRecursiveByType(std::string Type, Archive** Result, Archive* Node = nullptr);
		void FindNodesRecursiveByType(std::string Type, std::vector<Archive*>& Result, Archive* Node = nullptr);

	private:

		void ParseHeader();
		bool CheckIfDirectory(const ArchiveEntry& Entry);

	private:

		Archive* mParent;

		BinaryReader mBinaryReader = {};
		U16 mIndex = 0;
		U32 mOffset = 0;
		U32 mSize = 0;
		std::string mType = "";
		std::string mName = "";
		bool mIsDirectory = false;
		std::vector<ArchiveEntry> mEntries = {};
		std::vector<Archive*> mNodes = {};
	};
}
