#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

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

		void SetData(U8 const* Bytes, U64 Size);

	public:

		void Serialize(std::vector<U8>& Bytes);

		void Deserialize(U8 const* Bytes, U64 Size);
		void Deserialize(std::vector<U8> const& Bytes);

	public:

		void ExtractToDisk(fs::path const& File);
		void UnfoldToDisk(fs::path const& File);
		void PrintTableOfContent(std::function<void(char const*)> Callback, U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);
		void PrintOfType(std::function<void(char const*)> Callback, std::string const& Type);
		Archive* FindArchiveByType(std::string const& Type);
		Archive* FindArchiveByName(std::string const& Name);
		Archive* FindArchiveByTypeAndName(std::string const& Type, std::string const& Name);
		std::vector<Archive*> FindArchivesByType(std::string const& Type);
		std::vector<Archive*> FindArchivesByName(std::string const& Name);

	private:

		void SerializeRecursive();
		void DeserializeRecursive();

	private:

		void ExtractToDiskRecursive(fs::path File);
		void UnfoldToDiskRecursive(fs::path File);
		void PrintTableOfContentRecursive(std::function<void(char const*)> Callback, U32 Offset, U32 Indent, U32 Increment);
		void PrintOfTypeRecursive(std::function<void(char const*)> Callback, std::string const& Type);
		void FindArchiveByTypeRecursive(std::string const& Type, Archive** Result);
		void FindArchiveByNameRecursive(std::string const& Name, Archive** Result);
		void FindArchiveByTypeAndNameRecursive(std::string const& Type, std::string const& Name, Archive** Result);
		void FindArchivesByTypeRecursive(std::string const& Type, std::vector<Archive*>& Result);
		void FindArchivesByNameRecursive(std::string const& Name, std::vector<Archive*>& Result);

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