#include <Common/Macros.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

namespace ark
{
	static std::set<std::string> sKnownDirectoryTypes
	{
		"AKT", "BIN", "CMP", "DAT", "DDP", "EFF", "IDD", "SCP", "TBL",
	};

	static std::set<std::string> sKnownFileTypes
	{
		"A00", "A01", "ACT", "AK", "ANS",
		"B00", "B01", "BMH",
		"C00", "CAM", "CCH",
		"D00", "DDS",
		"EAR", "ECT", "EFP", "EMD", "EST",
		"FI2", "FIS",
		"ICO", "IDP", "ISL", "ITS",
		"JMP",
		"LI3",
		"MD", "MEH", "MOT", "MRT", "MSA", "MSD", "MSS",
		"PSN",
		"RHT", "RNI", "ROF",
		"S00", "S01", "S02", "S03", "S04", "S05", "SCA", "SCI", "SCL", "SCM", "SCR", "SEH", "SEQ", "SES", "SSD", "SSL",
		"TAT", "TRE", "TS", "TSC",
		"V00", "V01", "V02", "V03",
	};

	Archive::Archive(Archive* Parent)
		: mParent{ Parent }
	{

	}

	Archive::~Archive()
	{
		for (auto& node : mNodes)
		{
			delete node;
			node = nullptr;
		}
	}

	void Archive::LoadRecursive(const std::vector<U8>& Bytes, U16 Index, U32 Offset, U32 Size, const std::string& Type, const std::string& Name, bool IsDirectory)
	{
		mBinaryReader = Bytes;
		mIndex = Index;
		mOffset = Offset;
		mSize = Size;
		mType = Type;
		mName = Name;
		mIsDirectory = IsDirectory;

		if (mIsDirectory)
		{
			ParseHeader();

			for (U16 i = 0; i < (U16)mEntries.size(); i++)
			{
				mNodes.emplace_back(new Archive{ this })->LoadRecursive(
					mBinaryReader.Bytes(mEntries[i].Size, mEntries[i].Offset),
					i,
					mEntries[i].Offset,
					mEntries[i].Size,
					mEntries[i].Type,
					mEntries[i].Name,
					CheckIfDirectory(mEntries[i])
				);
			}
		}
	}

	void Archive::SaveRecursive()
	{
		
	}

	void Archive::DumpToFileSystemRecursive(fs::path File, Archive* Node)
	{
		if (!Node)
		{
			Node = this;
		}

		if (Node->mIsDirectory)
		{
			File /= Node->mType;

			FsUtils::CreateIfNotExists(File);

			for (const auto& node : Node->mNodes)
			{
				DumpToFileSystemRecursive(File, node);
			}
		}
		else
		{
			if (Node->mName == "")
			{
				File /= std::to_string(Node->mIndex) + "." + Node->mType;
			}
			else
			{
				File /= Node->mName + "." + Node->mType;
			}

			FsUtils::WriteBinary(File, Node->GetBytesWithoutHeader());
		}
	}

	void Archive::DumpToConsoleRecursive(U32 Offset, U32 Indent, U32 Increment, Archive* Node)
	{
		if (!Node)
		{
			Node = this;
		}

		if (Node->mParent)
		{
			for (U32 i = 0; i < Indent; i++)
			{
				LOG("%c", ((i % Increment == 0) && (i >= (Offset + Increment))) ? '|' : ' ');
			}

			LOG("%05u # %-20s # %-4s\n", Node->mIndex, Node->mName.c_str(), Node->mType.c_str());
		}

		if (Node->mIsDirectory)
		{
			for (const auto& node : Node->mNodes)
			{
				DumpToConsoleRecursive(Offset, Indent + Increment, Increment, node);
			}
		}
	}

	void Archive::FindNodeRecursiveByType(std::string Type, Archive** Result, Archive* Node)
	{
		if (!Node)
		{
			Node = this;
		}

		if (Node->mIsDirectory)
		{
			for (const auto& node : Node->mNodes)
			{
				FindNodeRecursiveByType(Type, Result, node);
			}
		}
		else
		{
			if (Type == Node->mType)
			{
				(*Result) = Node;

				return;
			}
		}
	}

	void Archive::FindNodesRecursiveByType(std::string Type, std::vector<Archive*>& Result, Archive* Node)
	{
		if (!Node)
		{
			Node = this;
		}

		if (Node->mIsDirectory)
		{
			for (const auto& node : Node->mNodes)
			{
				FindNodesRecursiveByType(Type, Result, node);
			}
		}
		else
		{
			if (Type == Node->mType)
			{
				Result.emplace_back(Node);
			}
		}
	}

	void Archive::ParseHeader()
	{
		if (mParent)
		{
			mBinaryReader.SeekAbsolute(32);
		}
		else
		{
			mBinaryReader.SeekAbsolute(0);
		}

		mEntries.resize(mBinaryReader.Read<U32>());

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			mEntries[i].Offset = mBinaryReader.Read<U32>();
		}

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			mEntries[i].Type = StringUtils::RemoveNulls(mBinaryReader.String(4));
		}

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			if (mParent)
			{
				if (mType == "ROF")
				{
					mEntries[i].Offset -= 8;
				}
			}
			else
			{
				mEntries[i].Offset -= 32;
			}
		}

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			mBinaryReader.SeekAbsolute(mEntries[i].Offset);
			mBinaryReader.SeekRelative(12);

			mEntries[i].Name = StringUtils::RemoveNulls(mBinaryReader.String(20));
		}

		for (U32 i = 1; i < mEntries.size(); i++)
		{
			mEntries[i - 1].Size = mEntries[i].Offset - mEntries[i - 1].Offset;
		}

		mEntries[mEntries.size() - 1].Size = (U32)mBinaryReader.GetSize() - mEntries[mEntries.size() - 1].Offset;
	}

	bool Archive::CheckIfDirectory(const ArchiveEntry& Entry)
	{
		mBinaryReader.SeekAbsolute(Entry.Offset + 32);

		U32 size = mBinaryReader.Read<U32>();

		if (size > 0 && size < 65535)
		{
			for (U32 i = 0; i < size; i++)
			{
				if ((i * 4) >= ((U32)mBinaryReader.GetSize() - 4))
				{
					return false;
				}

				U32 offset = mBinaryReader.Read<U32>();

				if (offset >= mBinaryReader.GetSize())
				{
					return false;
				}
			}

			for (U32 i = 0; i < size; i++)
			{
				if ((i * 4) >= ((U32)mBinaryReader.GetSize() - 4))
				{
					return false;
				}

				std::string type = StringUtils::RemoveNulls(mBinaryReader.String(4));

				if (!sKnownDirectoryTypes.contains(type) && !sKnownFileTypes.contains(type))
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}
}
