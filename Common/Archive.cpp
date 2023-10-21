#include <Common/Macros.h>
#include <Common/Archive.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/StringUtils.h>

namespace ark
{
	static std::set<std::string> sKnownDirectoryTypes
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
		ComputeDirectorySizesRecursive();
		SerializeRecursive();

		return { mBytes, mBytes + mSize };
	}

	void Archive::DeSerialize(const std::vector<U8>& Bytes)
	{
		mBytes = new U8[Bytes.size()];
		mSize = Bytes.size();

		std::memcpy(mBytes, Bytes.data(), Bytes.size());

		DeSerializeRecursive();
		ComputeDirectorySizesRecursive();
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
		if (mIsDirectory)
		{
			if (mBytes)
			{
				delete[] mBytes;
				mBytes = new U8[mSize];
			}

			BinaryWriter writer = { mBytes, mSize };

			WriteDirectoryHeader(writer);

			for (const auto& archive : *this)
			{
				archive->SerializeRecursive();

				writer.String("AABBCCDD", 8);
				writer.String(archive->mType, 4);
				writer.String(archive->mName, 20);
				writer.Bytes(archive->mBytes, archive->mSize);
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
				File /= mType;

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
				prevSequenceIndex = i;
			}
		}

		if (entryCount)
		{
			Archive* lastArchive = (*this)[entryCount - 1];

			lastArchive->mSize = mSize - lastArchive->mParentOffset;
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

			archive->mBytes = new U8[archive->mSize];

			std::memset(archive->mBytes, 0, archive->mSize);
			std::memcpy(archive->mBytes, mBytes + archive->mParentOffset, archive->mSize);
		}
	}

	void Archive::WriteDirectoryHeader(BinaryWriter& Writer)
	{
		U16 entryCount = (U16)size();

		Writer.Write<U32>((U32)entryCount);

		U64 acc = 0;
		
		acc += 4 + ((entryCount * 2) * 4);

		acc = ALIGN_UP(acc, 16);

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

		Writer.AlignUp(16);
	}

	U64 Archive::ComputeDirectorySizesRecursive()
	{
		U64 acc = 0;

		if (mIsDirectory)
		{
			U16 entryCount = (U16)size();

			acc += 4 + ((entryCount * 2) * 4);

			acc = ALIGN_UP(acc, 16);

			for (const auto& archive : *this)
			{
				acc += FILE_HEADER_SIZE;
				acc += archive->ComputeDirectorySizesRecursive();
			}
		}
		else
		{
			acc = mSize;
		}

		return mSize = acc;
	}
}
