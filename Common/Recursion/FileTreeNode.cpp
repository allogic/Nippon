#include <Common/Recursion/FileTreeNode.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	FileTreeNode::FileTreeNode(const fs::path& File, U32 MaxDepth, U32 Depth)
		: mFile{ File }
		, mMaxDepth{ MaxDepth }
		, mDepth{ Depth }
	{
		if (mDepth < mMaxDepth)
		{
			if (fs::is_directory(mFile))
			{
				for (const auto& file : fs::directory_iterator{ mFile })
				{
					mNodes[file] = new FileTreeNode{ file, mMaxDepth, mDepth + 1 };
				}
			}
		}
	}

	FileTreeNode::~FileTreeNode()
	{
		for (auto& [file, node] : mNodes)
		{
			delete node;
			node = nullptr;
		}
	}
}