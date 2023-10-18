#include <Common/BinaryReader.h>
#include <Common/BinaryWriter.h>
#include <Common/Macros.h>

#include <Editor/Serializer/ScrSerializer.h>

#include <Editor/Structs/MdbStructs.h>
#include <Editor/Structs/MdStructs.h>

namespace ark
{
	ScrGroup ScrSerializer::FromBytes(const std::vector<U8>& Bytes)
	{
		ScrGroup group = {};

		BinaryReader reader = { Bytes.data(), Bytes.data() + Bytes.size() };

		ScrHeader scrHeader = reader.Read<ScrHeader>();

		group.Models.resize(scrHeader.SubMeshCount);

		std::vector<U32> transformOffsets = reader.Read<U32>(scrHeader.SubMeshCount);

		reader.SeekAbsolute(ALIGN_UP(reader.GetPosition(), 16));

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			U64 mdbStart = reader.GetPosition();

			ScrModel& scrModel = group.Models[i];

			scrModel.Index = i;
			scrModel.Entry.Header = reader.Read<MdbHeader>();

			if (scrModel.Entry.Header.MdbId == 0x0062646D)
			{
				scrModel.Entry.Divisions.resize(scrModel.Entry.Header.MeshDivisions);

				std::vector<U32> divisionOffsets = reader.Read<U32>(scrModel.Entry.Header.MeshDivisions);

				for (U16 j = 0; j < scrModel.Entry.Header.MeshDivisions; j++)
				{
					reader.SeekAbsolute(mdbStart + divisionOffsets[j]);

					U64 mdStart = reader.GetPosition();

					ScrDivision& scrDivision = scrModel.Entry.Divisions[j];

					scrDivision.Index = j;
					scrDivision.Header = reader.Read<MdHeader>();
					
					if (scrDivision.Header.VertexOffset != 0)
					{
						reader.SeekAbsolute(mdStart + scrDivision.Header.VertexOffset);
						reader.Read<ScrVertex>(scrDivision.Vertices, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.TextureMapOffset != 0)
					{
						reader.SeekAbsolute(mdStart + scrDivision.Header.TextureMapOffset);
						reader.Read<ScrTextureMap>(scrDivision.TextureMaps, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.TextureUvOffset != 0)
					{
						reader.SeekAbsolute(mdStart + scrDivision.Header.TextureUvOffset);
						reader.Read<ScrUv>(scrDivision.TextureUvs, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.ColorWeightOffset != 0)
					{
						reader.SeekAbsolute(mdStart + scrDivision.Header.ColorWeightOffset);
						reader.Read<ScrColorWeight>(scrDivision.ColorWeights, scrDivision.Header.VertexCount);
					}
				}
			}

			reader.SeekAbsolute(ALIGN_UP(reader.GetPosition(), 16));
		}

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			reader.SeekAbsolute(transformOffsets[i]);

			ScrModel& scrModel = group.Models[i];

			scrModel.Transform = reader.Read<ScrTransform>();
		}

		return group;
	}
}
