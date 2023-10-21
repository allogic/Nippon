#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <filesystem>

#include <Common/Types.h>
#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#define FILE_HEADER_SIZE 32

namespace ark
{
	namespace fs = std::filesystem;

	class Archive : private std::vector<Archive*>
	{
	public:

		Archive();
		Archive(Archive* Parent);
		virtual ~Archive();

	public:

		inline const auto& IsDirectory() const { return mIsDirectory; }

		inline const auto& GetBytes() const { return mBytes; }
		inline const auto& GetSize() const { return mSize; }
		inline const auto& GetParent() const { return mParent; }
		inline const auto& GetName() const { return mName; }
		inline const auto& GetType() const { return mType; }
		inline const auto& GetParentOffset() const { return mParentOffset; }
		inline const auto& GetParentIndex() const { return mParentIndex; }

	public:

		std::vector<U8> Serialize();
		void DeSerialize(const std::vector<U8>& Bytes);

		void ExtractToDisk(fs::path File);
		void UnfoldToDisk(fs::path File);
		void PrintTableOfContent(U32 Offset = 0, U32 Indent = 0, U32 Increment = 4);
		void FindArchiveByType(std::string Type, Archive** Result);
		void FindArchivesByType(std::string Type, std::vector<Archive*>& Result);

	private:

		void SerializeRecursive();
		void DeSerializeRecursive();

		void ExtractToDiskRecursive(fs::path File);
		void UnfoldToDiskRecursive(fs::path File);
		void PrintTableOfContentRecursive(U32 Offset, U32 Indent, U32 Increment);
		void FindArchiveByTypeRecursive(std::string Type, Archive** Result);
		void FindArchivesByTypeRecursive(std::string Type, std::vector<Archive*>& Result);

	private:

		bool CheckIfDirectory(BinaryReader& Reader);
		void CreateDirectory(BinaryReader& Reader);

	private:

		void WriteDirectoryHeader(BinaryWriter& Writer);

	private:

		U64 ComputeDirectorySizesRecursive();

	private:

		U8* mBytes = nullptr;
		U64 mSize = 0;

		Archive* mParent = nullptr;

		std::string mName = "";
		std::string mType = "";
		U32 mParentOffset = 0;
		U16 mParentIndex = 0;

		bool mIsDirectory = false;
	};
}
