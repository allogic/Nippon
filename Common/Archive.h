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

		Archive();
		Archive(const std::vector<U8>& Bytes);
		Archive(Archive* Parent);
		virtual ~Archive();

	public:

		void Load();
		void LoadRecursive(const U8* Start, const U8* End, U16 Index, U32 Offset, U32 Size, const std::string& Type, const std::string& Name);

		void SaveRecursive();

	public:

		inline const auto& GetName() const { return mName; }
		inline const auto& GetType() const { return mType; }
		inline const auto& GetOffset() const { return mOffset; }
		inline const auto& GetSize() const { return mSize; }

		inline const auto GetBytes() const { return mReader.BytesFrom(mSize, 0); }

	public:

		inline void SetBytes(const std::vector<U8>& Bytes) { mBytes = Bytes; }

	public:

		void DumpToDiskRecursive(fs::path File, Archive* Node = nullptr);
		void DumpTableOfContent(U32 Offset, U32 Indent, U32 Increment, Archive* Node = nullptr);
		void FindNodeRecursiveByType(std::string Type, Archive** Result, Archive* Node = nullptr);
		void FindNodesRecursiveByType(std::string Type, std::vector<Archive*>& Result, Archive* Node = nullptr);

	private:

		void ParseHeader();

		bool CheckIfOutOfBounds();
		bool CheckIfDirectory();

	private:

		std::vector<U8> mBytes = {};

		Archive* mParent = nullptr;

		const U8* mStart = nullptr;
		const U8* mEnd = nullptr;

		BinaryReader mReader = {};

		U16 mIndex = 0;
		U32 mOffset = 0;
		U32 mSize = 0;
		std::string mType = "";
		std::string mName = "";

		bool mIsOutOfBounds = false;
		bool mIsDirectory = false;

		std::vector<ArchiveEntry> mEntries = {};
		std::vector<Archive*> mNodes = {};
	};
}
