#include <Common/BinaryMediator.h>

#include <Editor/Serializer/MdSerializer.h>

#include <Editor/Structs/MdbStructs.h>
#include <Editor/Structs/ScrStructs.h>

namespace ark
{
	MdGroup MdSerializer::FromBytes(U8* Bytes, U64 Size)
	{
		MdGroup group = {};

		BinaryMediator mediator = { Bytes, Size };

		ScrHeader scrHeader = mediator.Read<ScrHeader>();

		group.Models.resize(scrHeader.SubMeshCount);

		std::vector<U32> transformOffsets = mediator.Read<U32>(scrHeader.SubMeshCount);

		mediator.AlignUp(16);

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			U64 mdbStart = mediator.GetPosition();

			MdModel& mdModel = group.Models[i];

			mdModel.Index = i;
			mdModel.Entry.Header = mediator.Read<MdbHeader>();

			if (mdModel.Entry.Header.MdbId == 0x0062646D)
			{
				mdModel.Entry.Divisions.resize(mdModel.Entry.Header.MeshDivisions);

				std::vector<U32> divisionOffsets = mediator.Read<U32>(mdModel.Entry.Header.MeshDivisions);

				for (U16 j = 0; j < mdModel.Entry.Header.MeshDivisions; j++)
				{
					mediator.SeekAbs(mdbStart + divisionOffsets[j]);

					U64 mdStart = mediator.GetPosition();

					MdDivision& mdDivision = mdModel.Entry.Divisions[j];

					mdDivision.Index = j;
					mdDivision.Header = mediator.Read<MdHeader>();

					if (mdDivision.Header.VertexOffset != 0)
					{
						mediator.SeekAbs(mdStart + mdDivision.Header.VertexOffset);
						mediator.Read<MdVertex>(mdDivision.Vertices, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.TextureMapOffset != 0)
					{
						mediator.SeekAbs(mdStart + mdDivision.Header.TextureMapOffset);
						mediator.Read<MdTextureMap>(mdDivision.TextureMaps, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.TextureUvOffset != 0)
					{
						mediator.SeekAbs(mdStart + mdDivision.Header.TextureUvOffset);
						mediator.Read<MdUv>(mdDivision.TextureUvs, mdDivision.Header.VertexCount);
					}

					if (mdDivision.Header.ColorWeightOffset != 0)
					{
						mediator.SeekAbs(mdStart + mdDivision.Header.ColorWeightOffset);
						mediator.Read<MdColorWeight>(mdDivision.ColorWeights, mdDivision.Header.VertexCount);
					}
				}
			}

			mediator.AlignUp(16);
		}

		for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
		{
			mediator.SeekAbs(transformOffsets[i]);

			MdModel& mdModel = group.Models[i];

			mdModel.Transform = mediator.Read<MdTransform>();
		}

		return group;
	}
}
