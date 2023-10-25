#include <Common/BinaryMediator.h>

#include <Editor/Serializer/ScrSerializer.h>

#include <Editor/Structs/MdbStructs.h>
#include <Editor/Structs/MdStructs.h>

namespace ark
{
	ScrGroup ScrSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		ScrGroup group = {};

		BinaryMediator mediator = { Bytes, Size };

		ScrHeader scrHeader = mediator.Read<ScrHeader>();

		group.Models.resize(scrHeader.SubMeshCount);

		std::vector<U32> transformOffsets = mediator.Read<U32>(scrHeader.SubMeshCount);

		mediator.AlignUp(16);

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			U64 mdbStart = mediator.GetPosition();

			ScrModel& scrModel = group.Models[i];

			scrModel.Index = i;
			scrModel.Entry.Header = mediator.Read<MdbHeader>();

			if (scrModel.Entry.Header.MdbId == 0x0062646D)
			{
				scrModel.Entry.Divisions.resize(scrModel.Entry.Header.MeshDivisions);

				std::vector<U32> divisionOffsets = mediator.Read<U32>(scrModel.Entry.Header.MeshDivisions);

				for (U16 j = 0; j < scrModel.Entry.Header.MeshDivisions; j++)
				{
					mediator.SeekAbs(mdbStart + divisionOffsets[j]);

					U64 mdStart = mediator.GetPosition();

					ScrDivision& scrDivision = scrModel.Entry.Divisions[j];

					scrDivision.Index = j;
					scrDivision.Header = mediator.Read<MdHeader>();
					
					if (scrDivision.Header.VertexOffset != 0)
					{
						mediator.SeekAbs(mdStart + scrDivision.Header.VertexOffset);
						mediator.Read<ScrVertex>(scrDivision.Vertices, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.TextureMapOffset != 0)
					{
						mediator.SeekAbs(mdStart + scrDivision.Header.TextureMapOffset);
						mediator.Read<ScrTextureMap>(scrDivision.TextureMaps, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.TextureUvOffset != 0)
					{
						mediator.SeekAbs(mdStart + scrDivision.Header.TextureUvOffset);
						mediator.Read<ScrUv>(scrDivision.TextureUvs, scrDivision.Header.VertexCount);
					}

					if (scrDivision.Header.ColorWeightOffset != 0)
					{
						mediator.SeekAbs(mdStart + scrDivision.Header.ColorWeightOffset);
						mediator.Read<ScrColorWeight>(scrDivision.ColorWeights, scrDivision.Header.VertexCount);
					}
				}
			}

			mediator.AlignUp(16);
		}

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			mediator.SeekAbs(transformOffsets[i]);

			ScrModel& scrModel = group.Models[i];

			scrModel.Transform = mediator.Read<ScrTransform>();
		}

		return group;
	}
}
