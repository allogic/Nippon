#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>

#define FILE_ALIGNMENT 256
#define FILE_HEADER_SIZE 32

namespace Nippon
{
	class Archive
	{
	public:

		Archive();
		Archive(Archive* Parent);
		virtual ~Archive();

	public:

		inline auto const& IsDirectory() const { return mIsDirectory; }

		inline auto const& GetBytes() const { return mBytes; }
		inline auto const& GetSize() const { return mSize; }
		inline auto const& GetParent() const { return mParent; }
		inline auto const& GetType() const { return mType; }
		inline auto const& GetName() const { return mName; }
		inline auto const& GetParentOffset() const { return mParentOffset; }
		inline auto const& GetParentIndex() const { return mParentIndex; }

	public:

		std::vector<U8> Serialize();
		void DeSerialize(std::vector<U8> const& Bytes);

		void ExtractToDisk(fs::path const& File);
		void UnfoldToDisk(fs::path const& File);
		void PrintTableOfContent(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);
		Archive* FindArchiveByType(std::string const& Type);
		std::vector<Archive*> FindArchivesByType(std::string const& Type);

	private:

		void SerializeRecursive();
		void DeSerializeRecursive();

		void ExtractToDiskRecursive(fs::path File);
		void UnfoldToDiskRecursive(fs::path File);
		void PrintTableOfContentRecursive(U32 Offset, U32 Indent, U32 Increment);
		void FindArchiveByTypeRecursive(std::string const& Type, Archive** Result);
		void FindArchivesByTypeRecursive(std::string const& Type, std::vector<Archive*>& Result);

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

		std::vector<Archive*> mChildren = {};

		I8 mType[4 + 1] = "";
		I8 mName[20 + 1] = "";
		U32 mParentOffset = 0;
		U16 mParentIndex = 0;

		bool mIsDirectory = false;
	};
}