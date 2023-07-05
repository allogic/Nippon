#include <Common/Alignment.h>

#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Serializer/MdSerializer.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	MdGroup MdSerializer::FromFile(const fs::path& File)
	{
		MdGroup group = {};

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

			MdModel& mdModel = group.Models[i];

			mdModel.Index = i;
			mdModel.Entry.Header = binaryReader.Read<MdbHeader>();

			if (mdModel.Entry.Header.MdbId == 0x0062646D)
			{
				mdModel.Entry.Divisions.resize(mdModel.Entry.Header.MeshDivisions);

				std::vector<U32> divisionOffsets = binaryReader.Read<U32>(mdModel.Entry.Header.MeshDivisions);

				for (U16 j = 0; j < mdModel.Entry.Header.MeshDivisions; j++)
				{
					binaryReader.SeekAbsolute(mdbStart + divisionOffsets[j]);

					U64 mdStart = binaryReader.GetPosition();

					MdDivision& mdDivision = mdModel.Entry.Divisions[j];

					mdDivision.Index = j;
					mdDivision.Header = binaryReader.Read<MdHeader>();

					if (mdDivision.Header.VertexOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + mdDivision.Header.VertexOffset);
						binaryReader.Read<MdVertex>(mdDivision.Vertices, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.TextureMapOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + mdDivision.Header.TextureMapOffset);
						binaryReader.Read<MdTextureMap>(mdDivision.TextureMaps, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.TextureUvOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + mdDivision.Header.TextureUvOffset);
						binaryReader.Read<MdUv>(mdDivision.TextureUvs, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.ColorWeightOffset != 0)
					{
						binaryReader.SeekAbsolute(mdStart + mdDivision.Header.ColorWeightOffset);
						binaryReader.Read<MdColorWeight>(mdDivision.ColorWeights, mdDivision.Header.VertexCount);
					}
				}
			}

			binaryReader.SeekAbsolute(Align<16>::Up(binaryReader.GetPosition()));
		}

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			binaryReader.SeekAbsolute(transformOffsets[i]);

			MdModel& mdModel = group.Models[i];

			mdModel.Transform = binaryReader.Read<MdTransform>();
		}

		return group;
	}

	void MdSerializer::ToFile(const fs::path& File, const MdGroup& Group)
	{
		BinaryWriter binaryWriter = {};



		FsUtils::WriteBinary(File, binaryWriter.GetBytes());
	}
}