#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>

#include <Editor/Serializer/MdSerializer.h>

#include <Editor/Structs/MdbStructs.h>
#include <Editor/Structs/ScrStructs.h>

namespace ark
{
	MdGroup MdSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		MdGroup group = {};

		BinaryReader reader = { Bytes, Size };

		ScrHeader scrHeader = reader.Read<ScrHeader>();

		group.Models.resize(scrHeader.SubMeshCount);

		std::vector<U32> transformOffsets = reader.Read<U32>(scrHeader.SubMeshCount);

		reader.AlignUp(16);

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			U64 mdbStart = reader.GetPosition();

			MdModel& mdModel = group.Models[i];

			mdModel.Index = i;
			mdModel.Entry.Header = reader.Read<MdbHeader>();

			if (mdModel.Entry.Header.MdbId == 0x0062646D)
			{
				mdModel.Entry.Divisions.resize(mdModel.Entry.Header.MeshDivisions);

				std::vector<U32> divisionOffsets = reader.Read<U32>(mdModel.Entry.Header.MeshDivisions);

				for (U16 j = 0; j < mdModel.Entry.Header.MeshDivisions; j++)
				{
					reader.SeekAbs(mdbStart + divisionOffsets[j]);

					U64 mdStart = reader.GetPosition();

					MdDivision& mdDivision = mdModel.Entry.Divisions[j];

					mdDivision.Index = j;
					mdDivision.Header = reader.Read<MdHeader>();

					if (mdDivision.Header.VertexOffset != 0)
					{
						reader.SeekAbs(mdStart + mdDivision.Header.VertexOffset);
						reader.Read<MdVertex>(mdDivision.Vertices, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.TextureMapOffset != 0)
					{
						reader.SeekAbs(mdStart + mdDivision.Header.TextureMapOffset);
						reader.Read<MdTextureMap>(mdDivision.TextureMaps, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.TextureUvOffset != 0)
					{
						reader.SeekAbs(mdStart + mdDivision.Header.TextureUvOffset);
						reader.Read<MdUv>(mdDivision.TextureUvs, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.ColorWeightOffset != 0)
					{
						reader.SeekAbs(mdStart + mdDivision.Header.ColorWeightOffset);
						reader.Read<MdColorWeight>(mdDivision.ColorWeights, mdDivision.Header.VertexCount);
					}
				}
			}

			reader.AlignUp(16);
		}

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			reader.SeekAbs(transformOffsets[i]);

			MdModel& mdModel = group.Models[i];

			mdModel.Transform = reader.Read<MdTransform>();
		}

		return group;
	}
}
