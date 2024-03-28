#include <Memory.h>

#include <Archive/Archive.h>

#include <Common/BinaryMediator.h>

#include <Interface/Log.h>

#include <Utility/FileUtility.h>
#include <Utility/StringUtility.h>

namespace Nippon
{
	static std::set<std::string> sKnownDirectoryTypes =
	{
		"", // Empty directory types exist
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

	static std::set<std::string> sKnownFileTypes =
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

	Archive::Archive(Archive* Parent)
		: mParent{ Parent }
	{

	}

	Archive::~Archive()
	{
		for (auto const& child : mChildren)
		{
			child->~Archive();

			Memory::Free(child);
		}

		Memory::Free(mBytes);
	}

	std::vector<U8> Archive::Serialize()
	{
		UpdateSizesRecursive();
		UpdateByteArraysRecursive();

		SerializeRecursive();

		return { mBytes, mBytes + mSize };
	}

	void Archive::DeSerialize(std::vector<U8> const& Bytes)
	{
		mBytes = (U8*)Memory::Alloc(Bytes.size(), Bytes.data());
		mSize = Bytes.size();

		DeSerializeRecursive();

		UpdateSizesRecursive();
		UpdateByteArraysRecursive();
	}

	void Archive::ExtractToDisk(fs::path const& File)
	{
		ExtractToDiskRecursive(File);
	}

	void Archive::UnfoldToDisk(fs::path const& File)
	{
		UnfoldToDiskRecursive(File);
	}

	void Archive::PrintTableOfContent(U32 Offset, U32 Indent, U32 Increment)
	{
		Log::Add("Table Of Content\n");

		PrintTableOfContentRecursive(Offset, Indent, Increment);
	}

	Archive* Archive::FindArchiveByType(std::string const& Type)
	{
		Archive* result = nullptr;

		FindArchiveByTypeRecursive(Type, &result);

		return result;
	}

	std::vector<Archive*> Archive::FindArchivesByType(std::string const& Type)
	{
		std::vector<Archive*> result = {};

		FindArchivesByTypeRecursive(Type, result);

		return result;
	}

	void Archive::SerializeRecursive()
	{
		BinaryMediator mediator = { mBytes, mSize };

		if (mIsDirectory)
		{
			WriteDirectoryHeader(&mediator);

			for (auto const& child : mChildren)
			{
				child->SerializeRecursive();

				WriteFileContent(&mediator, child);
			}
		}
		else
		{
			if (std::strcmp(mType, "ROF") == 0)
			{
				mediator.WriteStringRange("RUNOFS64", 8);

				U64 acc = ALIGN_UP_BY(4 + ((mParent->mChildren.size() * 2) * 4), FILE_ALIGNMENT);

				for (auto const& child : mParent->mChildren)
				{
					acc += FILE_HEADER_SIZE;

					mediator.Write<U32>((U32)acc);
					mediator.SeekRel(4);

					acc += child->mSize;
				}
			}
		}
	}

	void Archive::DeSerializeRecursive()
	{
		BinaryMediator mediator = { mBytes, mSize };

		mIsDirectory = CheckIfDirectory(&mediator);

		if (mIsDirectory)
		{
			CreateDirectory(&mediator);

			for (auto const& child : mChildren)
			{
				child->DeSerializeRecursive();
			}
		}
	}

	void Archive::ExtractToDiskRecursive(fs::path File)
	{
		if (mIsDirectory)
		{
			if (mParent)
			{
				std::string name = mName;
				std::string type = mType;

				if (name != "" && type != "")
				{
					File /= name + "." + type;
				}
				else if (name != "" && type == "")
				{
					File /= name;
				}
				else if (name == "" && type != "")
				{
					File /= std::to_string(mParentIndex) + "." + type;
				}
				else if (name == "" && type == "")
				{
					File /= std::to_string(mParentIndex);
				}

				FileUtility::CreateDirIfNotExist(File, true);
			}

			for (auto const& child : mChildren)
			{
				child->ExtractToDiskRecursive(File);
			}
		}
		else
		{
			if (mSize)
			{
				std::string name = mName;
				std::string type = mType;

				if (name != "" && type != "")
				{
					File /= name + "." + type;
				}
				else if (name != "" && type == "")
				{
					File /= name;
				}
				else if (name == "" && type != "")
				{
					File /= std::to_string(mParentIndex) + "." + type;
				}
				else if (name == "" && type == "")
				{
					File /= std::to_string(mParentIndex);
				}

				FileUtility::WriteBinary(File, mBytes, mSize);
			}
		}
	}

	void Archive::UnfoldToDiskRecursive(fs::path File)
	{
		if (mIsDirectory)
		{
			for (auto const& child : mChildren)
			{
				child->UnfoldToDiskRecursive(File);
			}

			if (mParent)
			{
				if (mSize)
				{
					std::string name = mName;
					std::string type = mType;

					if (name != "" && type != "")
					{
						File /= name + "." + type;
					}
					else if (name != "" && type == "")
					{
						File /= name;
					}
					else if (name == "" && type != "")
					{
						File /= std::to_string(mParentIndex) + "." + type;
					}
					else if (name == "" && type == "")
					{
						File /= std::to_string(mParentIndex);
					}

					FileUtility::WriteBinary(File, mBytes, mSize);
				}
			}
		}
	}

	void Archive::PrintTableOfContentRecursive(U32 Offset, U32 Indent, U32 Increment)
	{
		if (mParent)
		{
			for (U32 i = 0; i < Offset; i++)
			{
				Log::Add(" ");
			}

			for (U32 i = 0; i < Indent; i++)
			{
				Log::Add("%c", ((i % Increment == 0) && (i >= Increment)) ? '|' : ' ');
			}

			U64 kiloByteInteger = mSize / 1000;
			U64 kiloByteFraction = mSize % 1000;

			Log::Add("0x%08X # %05u # %-20s # %-4s # %llu.%llu KB\n", mParentOffset, mParentIndex, mName, mType, kiloByteInteger, kiloByteFraction);
		}

		if (mIsDirectory)
		{
			for (auto const& child : mChildren)
			{
				child->PrintTableOfContentRecursive(Offset, Indent + Increment, Increment);
			}
		}
	}

	void Archive::FindArchiveByTypeRecursive(std::string const& Type, Archive** Result)
	{
		if (mIsDirectory)
		{
			for (auto const& child : mChildren)
			{
				child->FindArchiveByTypeRecursive(Type, Result);
			}
		}
		else
		{
			if (std::strcmp(mType, Type.data()) == 0)
			{
				(*Result) = this;

				return;
			}
		}
	}

	void Archive::FindArchivesByTypeRecursive(std::string const& Type, std::vector<Archive*>& Result)
	{
		if (mIsDirectory)
		{
			for (auto const& child : mChildren)
			{
				child->FindArchivesByTypeRecursive(Type, Result);
			}
		}
		else
		{
			if (std::strcmp(mType, Type.data()) == 0)
			{
				Result.emplace_back(this);
			}
		}
	}

	bool Archive::CheckIfDirectory(BinaryMediator* Mediator)
	{
		if (sKnownDirectoryTypes.contains(mType))
		{
			Mediator->SeekAbs(0);

			U16 entryCount = (U16)Mediator->Read<U32>();

			if (entryCount > 0 && entryCount < 0xFFFF)
			{
				for (U32 i = 0; i < entryCount; i++)
				{
					if ((i * 4) >= (mSize - 4))
					{
						return false;
					}

					U32 offset = Mediator->Read<U32>();

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

					std::string type = StringUtility::RemoveChars(Mediator->ReadStringRange(4), '\0');

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

	void Archive::CreateDirectory(BinaryMediator* Mediator)
	{
		Mediator->SeekAbs(0);

		U16 entryCount = (U16)Mediator->Read<U32>();

		mChildren.resize(entryCount);

		for (U16 i = 0; i < entryCount; i++)
		{
			mChildren[i] = new (Memory::Alloc(sizeof(Archive))) Archive{ this };
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			child->mParentIndex = i;
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			child->mParentOffset = Mediator->Read<U32>();
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			std::string type = Mediator->ReadStringRange(4);

			std::memcpy(child->mType, type.data(), 4);
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			if (child->mParentOffset)
			{
				Mediator->SeekAbs(child->mParentOffset - FILE_HEADER_SIZE);
				Mediator->SeekRel(FILE_HEADER_SIZE - 20);

				std::string name = Mediator->ReadStringRange(20);

				std::memcpy(child->mName, name.data(), 20);
			}
		}

		U16 prevSequenceIndex = 0;

		for (U16 i = 1; i < entryCount; i++)
		{
			Archive* currChild = mChildren[i];
			Archive* prevChild = mChildren[prevSequenceIndex];

			if (currChild->mParentOffset)
			{
				prevChild->mSize = currChild->mParentOffset - prevChild->mParentOffset;
				prevChild->mSizePrev = prevChild->mSize;

				prevSequenceIndex = i;
			}
		}

		if (entryCount)
		{
			Archive* lastChild = mChildren[entryCount - 1];

			lastChild->mSize = mSize - lastChild->mParentOffset;
			lastChild->mSizePrev = lastChild->mSize;
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			if (child->mSize >= FILE_HEADER_SIZE)
			{
				child->mSize -= FILE_HEADER_SIZE;
			}
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			child->mBytes = (U8*)Memory::Alloc(ALIGN_UP_BY(child->mSize, FILE_ALIGNMENT));

			std::memset(child->mBytes, 0, ALIGN_UP_BY(child->mSize, FILE_ALIGNMENT));
			std::memcpy(child->mBytes, mBytes + child->mParentOffset, child->mSize);

			child->mSize = ALIGN_UP_BY(child->mSize, FILE_ALIGNMENT);
		}
	}

	void Archive::WriteDirectoryHeader(BinaryMediator* Mediator)
	{
		U64 acc = 0;

		acc += ALIGN_UP_BY(4 + ((mChildren.size() * 2) * 4), FILE_ALIGNMENT);

		U16 entryCount = (U16)mChildren.size();

		Mediator->Write<U32>((U32)entryCount);

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			acc += FILE_HEADER_SIZE;

			Mediator->Write<U32>((U32)acc);

			acc += child->mSize;
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* child = mChildren[i];

			Mediator->WriteStringRange(child->mType, 4);
		}

		Mediator->AlignUp(FILE_ALIGNMENT);
	}

	void Archive::WriteFileContent(BinaryMediator* Mediator, Archive* Archive)
	{
		Mediator->Write<U64>(0x0);
		Mediator->WriteStringRange(Archive->mType, 4);
		Mediator->WriteStringRange(Archive->mName, 20);
		Mediator->WriteByteRange(Archive->mBytes, Archive->mSize);
	}

	U64 Archive::UpdateSizesRecursive()
	{
		U64 acc = 0;

		if (mIsDirectory)
		{
			acc += ALIGN_UP_BY(4 + ((mChildren.size() * 2) * 4), FILE_ALIGNMENT);

			for (auto const& child : mChildren)
			{
				acc += FILE_HEADER_SIZE;
				acc += child->UpdateSizesRecursive();
			}
		}
		else
		{
			if (std::strcmp(mType, "ROF") == 0)
			{
				acc = 8 + (mParent->mChildren.size() * 8);
			}
			else
			{
				acc = mSize;
			}
		}

		mSizePrev = mSize;
		mSize = acc;

		return mSize;
	}

	void Archive::UpdateByteArraysRecursive()
	{
		if (mBytes && (mSize != mSizePrev))
		{
			U8* bytes = (U8*)Memory::Alloc(mSize);

			std::memset(bytes, 0, mSize);
			std::memcpy(bytes, mBytes, std::min(mSize, mSizePrev));

			Memory::Free(mBytes);

			mBytes = bytes;
		}

		if (mIsDirectory)
		{
			for (auto const& child : mChildren)
			{
				child->UpdateByteArraysRecursive();
			}
		}
	}
}