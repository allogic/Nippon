#include <Common/Macros.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

namespace ark
{
	static std::set<std::string> sKnownDirectoryTypes
	{
		"AKT",
		"BIN",
		"CMP",
		"DAT", "DDP",
		"EFF",
		"IDD",
		"SCP",
		"TBL",
	};

	static std::set<std::string> sKnownFileTypes
	{
		//"", // I've seen empty extensions as well
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

	void Archive::Load(const std::string& Name)
	{
		LoadRecursive(mBytes.data(), mBytes.data() + mBytes.size(), 0, 0, 0, "", Name, true, true);
	}

	void Archive::LoadRecursive(const U8* Start, const U8* End, U16 Index, U32 Offset, U32 Size, const std::string& Type, const std::string& Name, bool InBounds, bool IsDirectory)
	{
		mStart = Start;
		mEnd = End;
		mReader = { Start, End };
		mIndex = Index;
		mOffset = Offset;
		mSize = Size;
		mType = Type;
		mName = Name;
		mInBounds = InBounds;
		mIsDirectory = IsDirectory;

		if (mIsDirectory)
		{
			ParseHeader();

			for (U16 i = 0; i < (U16)mEntries.size(); i++)
			{
				bool entryInBounds = CheckInBounds(mEntries[i]);
				bool entryIsDirectory = CheckIfDirectory(mEntries[i]);

				Archive* archive = mNodes.emplace_back(new Archive{ this });
				
				archive->LoadRecursive(
					mStart + mEntries[i].Offset,
					mStart + mEntries[i].Offset + mEntries[i].Size,
					i,
					mEntries[i].Offset,
					mEntries[i].Size,
					mEntries[i].Type,
					mEntries[i].Name,
					entryInBounds,
					entryIsDirectory
				);
			}
		}
	}

	void Archive::LoadAndCollectExtTypes(std::set<std::string>& DirExtTypes, std::set<std::string>& FileExtTypes, const std::string& Name)
	{
		LoadAndCollectExtTypesRecursive(DirExtTypes, FileExtTypes, mBytes.data(), mBytes.data() + mBytes.size(), 0, 0, 0, "", Name, true, true);
	}

	void Archive::LoadAndCollectExtTypesRecursive(std::set<std::string>& DirExtTypes, std::set<std::string>& FileExtTypes, const U8* Start, const U8* End, U16 Index, U32 Offset, U32 Size, const std::string& Type, const std::string& Name, bool InBounds, bool IsDirectory)
	{
		mStart = Start;
		mEnd = End;
		mReader = { Start, End };
		mIndex = Index;
		mOffset = Offset;
		mSize = Size;
		mType = Type;
		mName = Name;
		mInBounds = InBounds;
		mIsDirectory = IsDirectory;

		if (mIsDirectory)
		{
			ParseHeader();

			for (U16 i = 0; i < (U16)mEntries.size(); i++)
			{
				bool entryInBounds = CheckInBounds(mEntries[i]);
				bool entryIsDirectory = CheckIfDirectory(mEntries[i]);

				if (entryIsDirectory)
				{
					DirExtTypes.emplace(mEntries[i].Type);
				}
				else
				{
					FileExtTypes.emplace(mEntries[i].Type);
				}

				Archive* archive = mNodes.emplace_back(new Archive{ this });
				
				archive->LoadAndCollectExtTypesRecursive(
					DirExtTypes,
					FileExtTypes,
					mStart + mEntries[i].Offset,
					mStart + mEntries[i].Offset + mEntries[i].Size,
					i,
					mEntries[i].Offset,
					mEntries[i].Size,
					mEntries[i].Type,
					mEntries[i].Name,
					entryInBounds,
					entryIsDirectory
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
			File /= Node->mType;

			FsUtils::CreateIfNotExists(File);

			for (const auto& node : Node->mNodes)
			{
				DumpToDiskRecursive(File, node);
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

			LOG("%05u # %-20s # %-4s # %u.%u KB\n", Node->mIndex, Node->mName.c_str(), Node->mType.c_str(), Node->GetSize() / 1000, Node->GetSize() % 1000);
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
		if (mParent)
		{
			mReader.SeekAbsolute(32);
		}
		else
		{
			mReader.SeekAbsolute(0);
		}

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
			mReader.SeekAbsolute(mEntries[i].Offset);
			mReader.SeekRelative(12);

			mEntries[i].Name = StringUtils::RemoveNulls(mReader.String(20));
		}

		for (U32 i = 1; i < mEntries.size(); i++)
		{
			mEntries[i - 1].Size = mEntries[i].Offset - mEntries[i - 1].Offset;
		}

		mEntries[mEntries.size() - 1].Size = (U32)mReader.GetSize() - mEntries[mEntries.size() - 1].Offset;
	}

	bool Archive::CheckInBounds(const ArchiveEntry& Entry)
	{
		if ((Entry.Offset + Entry.Size) >= mReader.GetSize())
		{
			return false;
		}

		return true;
	}

	bool Archive::CheckIfDirectory(const ArchiveEntry& Entry)
	{
		mReader.SeekAbsolute(Entry.Offset + 32);

		U32 entryCount = mReader.Read<U32>();

		if (entryCount > 0 && entryCount < 65535)
		{
			for (U32 i = 0; i < entryCount; i++)
			{
				if ((i * 4) >= ((U32)mReader.GetSize() - 4))
				{
					return false;
				}

				U32 offset = mReader.Read<U32>();

				if (offset >= mReader.GetSize())
				{
					return false;
				}
			}

			for (U32 i = 0; i < entryCount; i++)
			{
				if ((i * 4) >= ((U32)mReader.GetSize() - 4))
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

		return false;
	}
}
