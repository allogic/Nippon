#include <Common/Alignment.h>

#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/ScrSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	ScrGroup ScrSerializer::FromFile(const fs::path& File)
	{
		ScrGroup group = {};

		U16 index = 0;
		std::string name = "";
		std::string type = "";

		FsUtils::FromArchiveName(File.stem().string(), index, name, type);

		BinaryReader binaryReader = { FsUtils::ReadBinary(File) };

		ScrHeader scrHeader = binaryReader.Read<ScrHeader>();

		std::string logicalIndex = {};

		logicalIndex.resize(5);

		std::snprintf(&logicalIndex[0], 6, "%05u", index);

		group.Name = logicalIndex + "@" + name;
		group.Models.resize(scrHeader.SubMeshCount);

		std::vector<U32> transformOffsets = binaryReader.Read<U32>(scrHeader.SubMeshCount);

		binaryReader.SeekAbsolute(Align<16>::Up(binaryReader.GetPosition()));

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			U64 mdbStart = binaryReader.GetPosition();

			ScrModel& scrModel = group.Models[i];

			scrModel.Index = i;
			scrModel.Entry.Header = binaryReader.Read<MdbHeader>();

			if (scrModel.Entry.Header.MdbId == 0x0062646D)
			{
				scrModel.Entry.Divisions.resize(scrModel.Entry.Header.MeshDivisions);

				std::vector<U32> divisionOffsets = binaryReader.Read<U32>(scrModel.Entry.Header.MeshDivisions);

				for (U16 j = 0; j < scrModel.Entry.Header.MeshDivisions; j++)
				{
					binaryReader.SeekAbsolute(mdbStart + divisionOffsets[j]);

					U64 mdStart = binaryReader.GetPosition();

					ScrDivision& scrDivision = scrModel.Entry.Divisions[j];

					scrDivision.Index = j;
					scrDivision.Header = binaryReader.Read<MdHeader>();
					
					if (scrDivision.Header.VertexOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + scrDivision.Header.VertexOffset);
						binaryReader.Read<ScrVertex>(scrDivision.Vertices, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.TextureMapOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + scrDivision.Header.TextureMapOffset);
						binaryReader.Read<ScrTextureMap>(scrDivision.TextureMaps, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.TextureUvOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + scrDivision.Header.TextureUvOffset);
						binaryReader.Read<ScrUv>(scrDivision.TextureUvs, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.ColorWeightOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + scrDivision.Header.ColorWeightOffset);
						binaryReader.Read<ScrColorWeight>(scrDivision.ColorWeights, scrDivision.Header.VertexCount);
					}
				}
			}

			binaryReader.SeekAbsolute(Align<16>::Up(binaryReader.GetPosition()));
		}

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			binaryReader.SeekAbsolute(transformOffsets[i]);

			ScrModel& scrModel = group.Models[i];

			scrModel.Transform = binaryReader.Read<ScrTransform>();
		}

		return group;
	}

	void ScrSerializer::ToFile(const fs::path& File, const ScrGroup& Group)
	{
		BinaryWriter binaryWriter = {};

		

		FsUtils::WriteBinary(File, binaryWriter.GetBytes());
	}
}