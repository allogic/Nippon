#pragma once

#include <string>
#include <cstring>
#include <filesystem>
#include <vector>
#include <set>

#include <Common/Types.h>

#define FILE_ALIGNMENT 256
#define FILE_HEADER_SIZE 32

namespace ark
{
	namespace fs = std::filesystem;

	class BinaryMediator;

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
		inline const auto& GetType() const { return mType; }
		inline const auto& GetName() const { return mName; }
		inline const auto& GetParentOffset() const { return mParentOffset; }
		inline const auto& GetParentIndex() const { return mParentIndex; }

	public:

		std::vector<U8> Serialize();
		void DeSerialize(const std::vector<U8>& Bytes);

		void ExtractToDisk(const fs::path& File);
		void UnfoldToDisk(const fs::path& File);
		void PrintTableOfContent(U32 Offset = 0, U32 Indent = 0, U32 Increment = 4);
		Archive* FindArchiveByType(const std::string& Type);
		std::vector<Archive*> FindArchivesByType(const std::string& Type);

	private:

		void SerializeRecursive();
		void DeSerializeRecursive();

		void ExtractToDiskRecursive(fs::path File);
		void UnfoldToDiskRecursive(fs::path File);
		void PrintTableOfContentRecursive(U32 Offset, U32 Indent, U32 Increment);
		void FindArchiveByTypeRecursive(const std::string& Type, Archive** Result);
		void FindArchivesByTypeRecursive(const std::string& Type, std::vector<Archive*>& Result);

	private:

		bool CheckIfDirectory(BinaryMediator* Mediator);
		void CreateDirectory(BinaryMediator* Mediator);

	private:

		void WriteDirectoryHeader(BinaryMediator* Mediator);
		void WriteFileContent(BinaryMediator* Mediator, Archive* Archive);

	private:

		U64 UpdateSizesRecursive();
		void UpdateByteArraysRecursive();

	private:

		U8* mBytes = nullptr;

		U64 mSize = 0;
		U64 mSizePrev = 0;

		Archive* mParent = nullptr;

		I8 mType[4 + 1] = "";
		I8 mName[20 + 1] = "";
		U32 mParentOffset = 0;
		U16 mParentIndex = 0;

		bool mIsDirectory = false;
	};
}
