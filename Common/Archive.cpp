#include <Common/Macros.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

namespace ark
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
		for (auto& archive : *this)
		{
			delete archive;
			archive = nullptr;
		}

		if (mBytes)
		{
			delete[] mBytes;
			mBytes = nullptr;
		}
	}

	std::vector<U8> Archive::Serialize()
	{
		UpdateSizesRecursive();
		UpdateByteArraysRecursive();
		SerializeRecursive();

		return { mBytes, mBytes + mSize };
	}

	void Archive::DeSerialize(const std::vector<U8>& Bytes)
	{
		mBytes = new U8[Bytes.size()];
		mSize = Bytes.size();

		std::memcpy(mBytes, Bytes.data(), Bytes.size());

		DeSerializeRecursive();
		UpdateSizesRecursive();
		UpdateByteArraysRecursive();
	}

	void Archive::ExtractToDisk(fs::path File)
	{
		ExtractToDiskRecursive(File);
	}

	void Archive::UnfoldToDisk(fs::path File)
	{
		UnfoldToDiskRecursive(File);
	}

	void Archive::PrintTableOfContent(U32 Offset, U32 Indent, U32 Increment)
	{
		LOG("\n");
		LOG(" Table Of Content\n");
		LOG("-----------------------------------------------------------------------------------------\n");

		PrintTableOfContentRecursive(Offset, Indent, Increment);

		LOG("\n");
	}

	void Archive::FindArchiveByType(std::string Type, Archive** Result)
	{
		FindArchiveByTypeRecursive(Type, Result);
	}

	void Archive::FindArchivesByType(std::string Type, std::vector<Archive*>& Result)
	{
		FindArchivesByTypeRecursive(Type, Result);
	}

	void Archive::SerializeRecursive()
	{
		BinaryWriter writer = { mBytes, mSize };

		if (mIsDirectory)
		{
			WriteDirectoryHeader(writer);

			for (const auto& archive : *this)
			{
				archive->SerializeRecursive();

				WriteFileContent(writer, archive);
			}
		}
		else
		{
			if (mType == "ROF")
			{
				writer.String("RUNOFS64", 8);

				U64 acc = ALIGN_UP(4 + ((mParent->size() * 2) * 4), FILE_ALIGNMENT);

				for (const auto& archive : *mParent)
				{
					acc += FILE_HEADER_SIZE;

					writer.Write<U32>((U32)acc);
					writer.SeekRel(4);

					acc += archive->mSize;
				}
			}
		}
	}

	void Archive::DeSerializeRecursive()
	{
		BinaryReader reader = { mBytes, mSize };

		mIsDirectory = CheckIfDirectory(reader);

		if (mIsDirectory)
		{
			CreateDirectory(reader);

			for (const auto& archive : *this)
			{
				archive->DeSerializeRecursive();
			}
		}
	}

	void Archive::ExtractToDiskRecursive(fs::path File)
	{
		if (mIsDirectory)
		{
			if (mParent)
			{
				if (mName != "" && mType != "")
				{
					File /= mName + "." + mType;
				}
				else if (mName != "" && mType == "")
				{
					File /= mName;
				}
				else if (mName == "" && mType != "")
				{
					File /= std::to_string(mParentIndex) + "." + mType;
				}
				else if (mName == "" && mType == "")
				{
					File /= std::to_string(mParentIndex);
				}

				FsUtils::CreateDirIfNotExist(File, true);
			}

			for (const auto& archive : *this)
			{
				archive->ExtractToDiskRecursive(File);
			}
		}
		else
		{
			if (mSize)
			{
				if (mName != "" && mType != "")
				{
					File /= mName + "." + mType;
				}
				else if (mName != "" && mType == "")
				{
					File /= mName;
				}
				else if (mName == "" && mType != "")
				{
					File /= std::to_string(mParentIndex) + "." + mType;
				}
				else if (mName == "" && mType == "")
				{
					File /= std::to_string(mParentIndex);
				}

				FsUtils::WriteBinary(File, mBytes, mSize);
			}
		}
	}

	void Archive::UnfoldToDiskRecursive(fs::path File)
	{
		if (mIsDirectory)
		{
			for (const auto& archive : *this)
			{
				archive->UnfoldToDiskRecursive(File);
			}

			if (mParent)
			{
				if (mSize)
				{
					if (mName != "" && mType != "")
					{
						File /= mName + "." + mType;
					}
					else if (mName != "" && mType == "")
					{
						File /= mName;
					}
					else if (mName == "" && mType != "")
					{
						File /= std::to_string(mParentIndex) + "." + mType;
					}
					else if (mName == "" && mType == "")
					{
						File /= std::to_string(mParentIndex);
					}

					FsUtils::WriteBinary(File, mBytes, mSize);
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
				LOG(" ");
			}

			for (U32 i = 0; i < Indent; i++)
			{
				LOG("%c", ((i % Increment == 0) && (i >= Increment)) ? '|' : ' ');
			}

			U64 kiloByteInteger = mSize / 1000;
			U64 kiloByteFraction = mSize % 1000;

			LOG("0x%08X # %05u # %-20s # %-4s # %llu.%llu KB\n", mParentOffset, mParentIndex, mName.c_str(), mType.c_str(), kiloByteInteger, kiloByteFraction);
		}

		if (mIsDirectory)
		{
			for (const auto& archive : *this)
			{
				archive->PrintTableOfContentRecursive(Offset, Indent + Increment, Increment);
			}
		}
	}

	void Archive::FindArchiveByTypeRecursive(std::string Type, Archive** Result)
	{
		if (mIsDirectory)
		{
			for (const auto& archive : *this)
			{
				archive->FindArchiveByTypeRecursive(Type, Result);
			}
		}
		else
		{
			if (Type == mType)
			{
				(*Result) = this;

				return;
			}
		}
	}

	void Archive::FindArchivesByTypeRecursive(std::string Type, std::vector<Archive*>& Result)
	{
		if (mIsDirectory)
		{
			for (const auto& archive : *this)
			{
				archive->FindArchivesByTypeRecursive(Type, Result);
			}
		}
		else
		{
			if (Type == mType)
			{
				Result.emplace_back(this);
			}
		}
	}

	bool Archive::CheckIfDirectory(BinaryReader& Reader)
	{
		if (sKnownDirectoryTypes.contains(mType))
		{
			Reader.SeekAbs(0);

			U16 entryCount = (U16)Reader.Read<U32>();

			if (entryCount > 0 && entryCount < 0xFFFF)
			{
				for (U32 i = 0; i < entryCount; i++)
				{
					if ((i * 4) >= (mSize - 4))
					{
						return false;
					}

					U32 offset = Reader.Read<U32>();

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

					std::string type = StringUtils::RemoveNulls(Reader.String(4));

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

	void Archive::CreateDirectory(BinaryReader& Reader)
	{
		Reader.SeekAbs(0);

		U16 entryCount = (U16)Reader.Read<U32>();

		resize(entryCount);

		for (U16 i = 0; i < entryCount; i++)
		{
			(*this)[i] = new Archive{ this };
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			archive->mParentIndex = i;
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			archive->mParentOffset = Reader.Read<U32>();
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			archive->mType = StringUtils::RemoveNulls(Reader.String(4));
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			if (archive->mParentOffset)
			{
				Reader.SeekAbs(archive->mParentOffset - FILE_HEADER_SIZE);
				Reader.SeekRel(FILE_HEADER_SIZE - 20);

				archive->mName = StringUtils::RemoveNulls(Reader.String(20));
			}
		}

		U16 prevSequenceIndex = 0;

		for (U16 i = 1; i < entryCount; i++)
		{
			Archive* currArchive = (*this)[i];
			Archive* prevArchive = (*this)[prevSequenceIndex];

			if (currArchive->mParentOffset)
			{
				prevArchive->mSize = currArchive->mParentOffset - prevArchive->mParentOffset;
				prevArchive->mSizePrev = prevArchive->mSize;

				prevSequenceIndex = i;
			}
		}

		if (entryCount)
		{
			Archive* lastArchive = (*this)[entryCount - 1];

			lastArchive->mSize = mSize - lastArchive->mParentOffset;
			lastArchive->mSizePrev = lastArchive->mSize;
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			if (archive->mSize >= FILE_HEADER_SIZE)
			{
				archive->mSize -= FILE_HEADER_SIZE;
			}
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			archive->mBytes = new U8[ALIGN_UP(archive->mSize, FILE_ALIGNMENT)];

			std::memset(archive->mBytes, 0, ALIGN_UP(archive->mSize, FILE_ALIGNMENT));
			std::memcpy(archive->mBytes, mBytes + archive->mParentOffset, archive->mSize);

			archive->mSize = ALIGN_UP(archive->mSize, FILE_ALIGNMENT);
		}
	}

	void Archive::WriteDirectoryHeader(BinaryWriter& Writer)
	{
		U64 acc = 0;

		acc += ALIGN_UP(4 + ((size() * 2) * 4), FILE_ALIGNMENT);

		Writer.ZeroNoInc(acc);

		U16 entryCount = (U16)size();

		Writer.Write<U32>((U32)entryCount);

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			acc += FILE_HEADER_SIZE;

			Writer.Write<U32>((U32)acc);

			acc += archive->mSize;
		}

		for (U16 i = 0; i < entryCount; i++)
		{
			Archive* archive = (*this)[i];

			Writer.String(archive->mType, 4);
		}

		Writer.AlignUp(FILE_ALIGNMENT);
	}

	void Archive::WriteFileContent(BinaryWriter& Writer, Archive* Archive)
	{
		Writer.Zero(8);
		Writer.String(Archive->mType, 4);
		Writer.String(Archive->mName, 20);
		Writer.Bytes(Archive->mBytes, Archive->mSize);
	}

	U64 Archive::UpdateSizesRecursive()
	{
		U64 acc = 0;

		if (mIsDirectory)
		{
			acc += ALIGN_UP(4 + ((size() * 2) * 4), FILE_ALIGNMENT);

			for (const auto& archive : *this)
			{
				acc += FILE_HEADER_SIZE;
				acc += archive->UpdateSizesRecursive();
			}
		}
		else
		{
			if (mType == "ROF")
			{
				acc = 8 + (mParent->size() * 8);
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
			U8* bytes = new U8[mSize];

			std::memset(bytes, 0, mSize);
			std::memcpy(bytes, mBytes, std::min(mSize, mSizePrev));

			delete[] mBytes;

			mBytes = bytes;
		}

		if (mIsDirectory)
		{
			for (const auto& archive : *this)
			{
				archive->UpdateByteArraysRecursive();
			}
		}
	}
}
