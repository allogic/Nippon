#pragma once

#include <filesystem>
#include <map>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class FileTreeNode
	{
	public:

		FileTreeNode(const fs::path& File, U32 MaxDepth, U32 Depth = 0);
		virtual ~FileTreeNode();

	public:

		inline auto IsDirectory() const { return fs::is_directory(mFile); }
		inline auto IsFile() const { return !fs::is_directory(mFile); }
		inline auto IsMaxDepth() const { return mDepth == mMaxDepth;  }

		inline auto GetParent() const { return mFile.parent_path(); }
		inline auto GetPath() const { return mFile; }
		inline auto GetName() const { return mFile.stem(); }
		inline auto GetExtension() const { return mFile.extension(); }
		inline auto GetSize() const { return fs::file_size(mFile); }

		inline auto& GetSubNodes() const { return mNodes; }

	private:

		fs::path mFile;
		U32 mMaxDepth;

		U32 mDepth = 0;
		std::map<fs::path, FileTreeNode*> mNodes = {};
	};
}