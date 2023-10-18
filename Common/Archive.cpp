#include <Common/Macros.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

namespace ark
{
	static std::set<std::string> sKnownDirectoryTypes
	{
		"", // Empty file types exist
		"AKT",
		"BIN",
		"CMP",
		"DAT", "DDP",
		"EFF",
		"IDD",
		"PAC",
		"SCP",
		"TBL",
	};

	static std::set<std::string> sKnownFileTypes
	{
		"", // Empty file types exist
		"A00", "A01", "ACT", "AK", "ANS", "ANC",
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
		"V00", "V01", "V02", "V03", "VET",
	};

	Archive::Archive()
	{

	}

	Archive::Archive(const std::vector<U8>& Bytes)
		: mBytes{ Bytes }
	{

	}

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

	void Archive::Load()
	{
		LoadRecursive(mBytes.data(), mBytes.data() + mBytes.size(), 0, 0, (U32)mBytes.size(), "", "");
	}

	void Archive::LoadRecursive(const U8* Start, const U8* End, U16 Index, U32 Offset, U32 Size, const std::string& Type, const std::string& Name)
	{
		mStart = Start;
		mEnd = End;

		mReader = { Start, End };

		mIndex = Index;
		mOffset = Offset;
		mSize = Size;
		mType = Type;
		mName = Name;

		mIsOutOfBounds = CheckIfOutOfBounds();
		mIsDirectory = CheckIfDirectory();

		if (!mIsOutOfBounds && mIsDirectory)
		{
			ParseHeader();

			for (U16 i = 0; i < (U16)mEntries.size(); i++)
			{
				Archive* archive = mNodes.emplace_back(new Archive{ this });
				
				archive->LoadRecursive(
					mStart + mEntries[i].Offset,
					mStart + mEntries[i].Offset + mEntries[i].Size,
					i,
					mEntries[i].Offset,
					mEntries[i].Size,
					mEntries[i].Type,
					mEntries[i].Name
				);
			}
		}
	}

	void Archive::SaveRecursive()
	{
		
	}

	void Archive::DumpToDiskRecursive(fs::path File, Archive* Node)
	{
		if (!Node)
		{
			Node = this;
		}

		if (Node->mIsDirectory)
		{
			if (Node->mParent)
			{
				File /= Node->mType;

				FsUtils::CreateIfNotExist(File, true);
			}

			for (const auto& node : Node->mNodes)
			{
				DumpToDiskRecursive(File, node);
			}
		}
		else
		{
			if (Node->mSize)
			{
				if (Node->mName != "" && Node->mType != "")
				{
					File /= Node->mName + "." + Node->mType;
				}
				else if (Node->mName != "" && Node->mType == "")
				{
					File /= Node->mName;
				}
				else if (Node->mName == "" && Node->mType != "")
				{
					File /= std::to_string(Node->mIndex) + "." + Node->mName;
				}

				FsUtils::WriteBinary(File, Node->GetBytes());
			}
		}
	}

	void Archive::DumpTableOfContent(U32 Offset, U32 Indent, U32 Increment, Archive* Node)
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

			U32 kiloByteInteger = Node->GetSize() / 1000;
			U32 kiloByteFraction = Node->GetSize() % 1000;

			LOG("%05u # %-20s # %-4s # %u.%u KB\n", Node->mIndex, Node->mName.c_str(), Node->mType.c_str(), kiloByteInteger, kiloByteFraction);
		}

		if (Node->mIsDirectory)
		{
			for (const auto& node : Node->mNodes)
			{
				DumpTableOfContent(Offset, Indent + Increment, Increment, node);
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
		mReader.SeekAbs(0);

		mEntries.resize(mReader.Read<U32>());

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			mEntries[i].Offset = mReader.Read<U32>();
		}

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			mEntries[i].Type = StringUtils::RemoveNulls(mReader.String(4));
		}

		for (U32 i = 0; i < mEntries.size(); i++)
		{
			if (mEntries[i].Offset)
			{
				mReader.SeekAbs(mEntries[i].Offset);
				mReader.SeekRel(-20);
		
				mEntries[i].Name = StringUtils::RemoveNulls(mReader.String(20));
			}
		}
		
		U32 prevSequenceIndex = 0;
		for (U32 i = 1; i < mEntries.size(); i++)
		{
			if (mEntries[i].Offset)
			{
				mEntries[prevSequenceIndex].Size = mEntries[i].Offset - mEntries[prevSequenceIndex].Offset;
				prevSequenceIndex = i;
			}
		}

		mEntries[mEntries.size() - 1].Size = mSize - mEntries[mEntries.size() - 1].Offset;
	}

	bool Archive::CheckIfOutOfBounds()
	{
		if (mParent)
		{
			if ((mStart < mParent->mStart) || (mStart > mParent->mEnd) ||
				(mEnd < mParent->mStart) || (mEnd > mParent->mEnd) ||
				(mStart > mEnd) || (mEnd < mStart))
			{
				return true;
			}
		}

		return false;
	}

	bool Archive::CheckIfDirectory()
	{
		if (sKnownDirectoryTypes.contains(mType))
		{
			mReader.SeekAbs(0);

			U32 entryCount = mReader.Read<U32>();

			if (entryCount > 0 && entryCount < 0xFFFF)
			{
				for (U32 i = 0; i < entryCount; i++)
				{
					if ((i * 4) >= (mSize - 4))
					{
						return false;
					}

					U32 offset = mReader.Read<U32>();

					if (offset >= mSize)
					{
						return false;
					}
				}

				for (U32 i = 0; i < entryCount; i++)
				{
					if ((i * 4) >= (mSize - 4))
					{
						return false;
					}

					std::string type = StringUtils::RemoveNulls(mReader.String(4));

					if (!sKnownDirectoryTypes.contains(type) && !sKnownFileTypes.contains(type))
					{
						return false;
					}
				}

				return true;
			}
		}

		return false;
	}
}
