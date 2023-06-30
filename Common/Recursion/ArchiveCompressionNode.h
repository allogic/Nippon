#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <Common/Types.h>
#include <Common/BinaryWriter.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class ArchiveCompressionNode
	{
	public:

		ArchiveCompressionNode(
			const fs::path& File,
			ArchiveCompressionNode* Parent,
			bool IsDat);
		virtual ~ArchiveCompressionNode();

	public:

		inline const auto& GetBytes() const { return mBinaryWriter.GetBytes(); }

	public:

		void CompressRecursive(U32 Count, ArchiveCompressionNode* Node = nullptr, bool Verbose = false);

	private:

		void WriteHeader();
		void WriteFile();

	private:

		void WriteIndices();
		void WriteTypes();

	private:

		fs::path mFile;
		ArchiveCompressionNode* mParent;
		bool mIsDat;

		BinaryWriter mBinaryWriter = {};
		bool mIsDirectory = false;
		U16 mIndex = 0;
		std::string mName = "";
		std::string mType = "";

		std::vector<ArchiveCompressionNode*> mNodes = {};
	};
}