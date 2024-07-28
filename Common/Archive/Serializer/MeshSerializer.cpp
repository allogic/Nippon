#include <Common/Archive/Serializer/MeshSerializer.h>

namespace Nippon
{
	std::vector<ScrMesh> MeshSerializer::ScrMeshesFromBytes(U8 const* Bytes, U64 Size)
	{
		std::vector<ScrMesh> meshes = {};

		BinaryMediator mediator = { Bytes, Size };

		ScrHeader scrHeader = mediator.Read<ScrHeader>();

		if (scrHeader.ScrId == SCR_HEADER_ID)
		{
			meshes.resize(scrHeader.SubMeshCount);

			std::vector<U32> transformOffsets = mediator.Read<U32>(scrHeader.SubMeshCount);

			mediator.AlignUp(16);

			for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
			{
				U64 mdbStart = mediator.GetPosition();

				auto& mesh = meshes[i];

				mesh.Index = i;
				mesh.Header = mediator.Read<MdbHeader>();

				if (mesh.Header.MdbId == MDB_HEADER_ID)
				{
					mesh.SubMeshes.resize(mesh.Header.MeshDivisions);

					std::vector<U32> subMeshOffsets = mediator.Read<U32>(mesh.Header.MeshDivisions);

					for (U16 j = 0; j < mesh.Header.MeshDivisions; j++)
					{
						mediator.SeekAbs(mdbStart + subMeshOffsets[j]);

						U64 mdStart = mediator.GetPosition();

						auto& subMesh = mesh.SubMeshes[j];

						subMesh.Index = j;
						subMesh.Header = mediator.Read<MdHeader>();

						if (subMesh.Header.VertexOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.VertexOffset);
							mediator.Read<ScrVertex>(subMesh.Vertices, subMesh.Header.VertexCount);
						}

						if (subMesh.Header.TextureMapOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.TextureMapOffset);
							mediator.Read<TextureMap>(subMesh.TextureMaps, subMesh.Header.VertexCount);
						}

						if (subMesh.Header.TextureUvOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.TextureUvOffset);
							mediator.Read<ScrUv>(subMesh.TextureUvs, subMesh.Header.VertexCount);
						}

						if (subMesh.Header.ColorWeightOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.ColorWeightOffset);
							mediator.Read<ColorWeight>(subMesh.ColorWeights, subMesh.Header.VertexCount);
						}
					}
				}

				mediator.AlignUp(16);
			}

			for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
			{
				mediator.SeekAbs(transformOffsets[i]);

				auto& mesh = meshes[i];

				mesh.Transform = mediator.Read<ScrTransform>();
			}
		}

		return meshes;
	}

	std::vector<MdMesh> MeshSerializer::MdMeshesFromBytes(U8 const* Bytes, U64 Size)
	{
		std::vector<MdMesh> meshes = {};

		BinaryMediator mediator = { Bytes, Size };

		ScrHeader scrHeader = mediator.Read<ScrHeader>();

		if (scrHeader.ScrId == SCR_HEADER_ID)
		{
			meshes.resize(scrHeader.SubMeshCount);

			std::vector<U32> transformOffsets = mediator.Read<U32>(scrHeader.SubMeshCount);

			mediator.AlignUp(16);

			for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
			{
				U64 mdbStart = mediator.GetPosition();

				auto& mesh = meshes[i];

				mesh.Index = i;
				mesh.Header = mediator.Read<MdbHeader>();

				if (mesh.Header.MdbId == MDB_HEADER_ID)
				{
					mesh.SubMeshes.resize(mesh.Header.MeshDivisions);

					std::vector<U32> subMeshOffsets = mediator.Read<U32>(mesh.Header.MeshDivisions);

					for (U16 j = 0; j < mesh.Header.MeshDivisions; j++)
					{
						mediator.SeekAbs(mdbStart + subMeshOffsets[j]);

						U64 mdStart = mediator.GetPosition();

						auto& subMesh = mesh.SubMeshes[j];

						subMesh.Index = j;
						subMesh.Header = mediator.Read<MdHeader>();

						if (subMesh.Header.VertexOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.VertexOffset);
							mediator.Read<MdVertex>(subMesh.Vertices, subMesh.Header.VertexCount);
						}

						if (subMesh.Header.TextureMapOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.TextureMapOffset);
							mediator.Read<TextureMap>(subMesh.TextureMaps, subMesh.Header.VertexCount);
						}

						if (subMesh.Header.TextureUvOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.TextureUvOffset);
							mediator.Read<MdUv>(subMesh.TextureUvs, subMesh.Header.VertexCount);
						}

						if (subMesh.Header.ColorWeightOffset != 0)
						{
							mediator.SeekAbs(mdStart + subMesh.Header.ColorWeightOffset);
							mediator.Read<ColorWeight>(subMesh.ColorWeights, subMesh.Header.VertexCount);
						}
					}
				}

				mediator.AlignUp(16);
			}

			for (U32 i = 0; i < scrHeader.SubMeshCount; i++)
			{
				mediator.SeekAbs(transformOffsets[i]);

				auto& mesh = meshes[i];

				mesh.Transform = mediator.Read<MdTransform>();
			}
		}

		return meshes;
	}
}