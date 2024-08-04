#include <Common/Macros.h>
#include <Common/Memory.h>
#include <Common/Magic.h>

#include <Common/Assimp/Importer.hpp>
#include <Common/Assimp/Exporter.hpp>
#include <Common/Assimp/postprocess.h>
#include <Common/Assimp/scene.h>

#include <Common/Model/Model.h>
#include <Common/Model/ModelInfo.h>

#include <Common/Misc/BinaryMediator.h>

#include <Common/Nlohmann/json.hpp>

#include <Common/Utility/AssimpUtility.h>
#include <Common/Utility/FileUtility.h>

#define PRINT(FMT, ...) \
	snprintf(buffer, sizeof(buffer), FMT, __VA_ARGS__); \
	Callback(buffer)

#define DEFAULT_ALIGNMENT 0x10

#define TRANSFORM_OFFSET_ALIGNMENT 0x10
#define SUBMESH_OFFSET_ALIGNMENT 0x10

#define MESH_ALIGNMENT 0x10
#define SUBMESH_ALIGNMENT 0x10
#define TRANSFORM_ALIGNMENT 0x10

#define EOF_ALIGNMENT 0x100

namespace Nippon
{
	void from_json(nlohmann::json const& Json, SubMeshRule& Rule)
	{
		Json.at("TextureIndex").get_to(Rule.TextureIndex);
	}

	void from_json(nlohmann::json const& Json, MeshRule& Rule)
	{
		Json.at("MdbId").get_to(Rule.MdbId);
		Json.at("MeshType").get_to(Rule.MeshType);
		Json.at("MeshId").get_to(Rule.MeshId);
		Json.at("SubMeshRules").get_to(Rule.SubMeshRules);
	}

	void from_json(nlohmann::json const& Json, ConversionRules& Rules)
	{
		Json.at("ScrId").get_to(Rules.ScrId);
		Json.at("FileType").get_to(Rules.FileType);
		Json.at("MeshRules").get_to(Rules.MeshRules);
	}

	void to_json(nlohmann::json& Json, SubMeshRule const& Rule)
	{
		Json =
		{
			{ "TextureIndex", Rule.TextureIndex },
		};
	}

	void to_json(nlohmann::json& Json, MeshRule const& Rule)
	{
		Json =
		{
			{ "MdbId", Rule.MdbId },
			{ "MeshType", Rule.MeshType },
			{ "MeshId", Rule.MeshId },
			{ "SubMeshRules", Rule.SubMeshRules },
		};
	}

	void to_json(nlohmann::json& Json, ConversionRules const& Rules)
	{
		Json =
		{
			{ "ScrId", Rules.ScrId },
			{ "FileType", Rules.FileType },
			{ "MeshRules", Rules.MeshRules },
		};
	}

	Model::Model()
	{
		
	}

	Model::~Model()
	{
		Memory::Free(mBytes);
	}

	void Model::SetData(U8 const* Bytes, U64 Size)
	{
		Memory::Free(mBytes);

		mBytes = (U8*)Memory::Alloc(Size);
		mSizePrev = mSize;
		mSize = Size;

		std::memcpy(mBytes, Bytes, Size);
	}

	void Model::Serialize(std::vector<U8>& Bytes)
	{
		UpdateSize();
		UpdateByteArray();

		SerializeModel();

		Bytes = { mBytes, mBytes + mSize };
	}

	void Model::Deserialize(U8 const* Bytes, U64 Size)
	{
		if (mBytes)
		{
			Memory::Free(mBytes);
		}

		mBytes = (U8*)Memory::Alloc(Size, Bytes);
		mSize = Size;

		DeserializeModel();

		UpdateSize();
		UpdateByteArray();
	}

	void Model::Deserialize(std::vector<U8> const& Bytes)
	{
		if (mBytes)
		{
			Memory::Free(mBytes);
		}

		mBytes = (U8*)Memory::Alloc(Bytes.size(), Bytes.data());
		mSize = Bytes.size();

		DeserializeModel();

		UpdateSize();
		UpdateByteArray();
	}

	void Model::CollectUniqueNonSerializableInfos(U8 const* Bytes, U64 Size, UniqueNonSerializableInfos& UniqueNonSerializableInfos)
	{
		if (mBytes)
		{
			Memory::Free(mBytes);
		}

		mBytes = (U8*)Memory::Alloc(Size, Bytes);
		mSize = Size;

		DeserializeModelAndCollectUniqueNonSerializableInfos(UniqueNonSerializableInfos);

		UpdateSize();
		UpdateByteArray();
	}

	void Model::CollectUniqueNonSerializableInfos(std::vector<U8> const& Bytes, UniqueNonSerializableInfos& UniqueNonSerializableInfos)
	{
		if (mBytes)
		{
			Memory::Free(mBytes);
		}

		mBytes = (U8*)Memory::Alloc(Bytes.size(), Bytes.data());
		mSize = Bytes.size();

		DeserializeModelAndCollectUniqueNonSerializableInfos(UniqueNonSerializableInfos);

		UpdateSize();
		UpdateByteArray();
	}

	void Model::PrintTableOfContent(std::function<void(char const*)> Callback)
	{
		static char buffer[1024] = {};

		PRINT("  ScrHeader:\n");
		PRINT("  | ScrId: 0x%08X\n", mScrHeader.ScrId);
		PRINT("  | FileType: 0x%08X\n", mScrHeader.FileType);
		PRINT("  | MeshCount: %u\n", mScrHeader.MeshCount);

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto const& mesh = mMdMeshes[i];

					PRINT("  | MdbHeader %u:\n", i);
					PRINT("  | | MdbId: 0x%08X\n", mesh.Header.MdbId);
					PRINT("  | | MeshType: 0x%08X\n", mesh.Header.MeshType);
					PRINT("  | | MeshId: 0x%04X\n", mesh.Header.MeshId);
					PRINT("  | | SubMeshCount: %u\n", mesh.Header.SubMeshCount);
					PRINT("  | | Transform:\n");
					PRINT("  | | | Position: [%.3f, %.3f, %.3f]\n", mesh.Transform.Position.x, mesh.Transform.Position.y, mesh.Transform.Position.z);
					PRINT("  | | | Rotation: [%.3f, %.3f, %.3f]\n", mesh.Transform.Rotation.x, mesh.Transform.Rotation.y, mesh.Transform.Rotation.z);
					PRINT("  | | | Scale: [%.3f, %.3f, %.3f]\n", mesh.Transform.Scale.x, mesh.Transform.Scale.y, mesh.Transform.Scale.z);

					for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
					{
						auto& subMesh = mesh.SubMeshes[j];

						U64 vertexStart = subMesh.Header.VertexOffset;
						U64 textureMapStart = subMesh.Header.TextureMapOffset;
						U64 textureUvStart = subMesh.Header.TextureUvOffset;
						U64 colorWeightStart = subMesh.Header.ColorWeightOffset;

						U64 vertexEnd = vertexStart + sizeof(MdVertex) * subMesh.Header.VertexCount;
						U64 textureMapEnd = textureMapStart + sizeof(TextureMap) * subMesh.Header.VertexCount;
						U64 textureUvEnd = textureUvStart + sizeof(MdUv) * subMesh.Header.VertexCount;
						U64 colorWeightEnd = colorWeightStart + sizeof(ColorWeight) * subMesh.Header.VertexCount;

						U64 vertexSize = vertexEnd - vertexStart;
						U64 textureMapSize = textureMapEnd - textureMapStart;
						U64 textureUvSize = textureUvEnd - textureUvStart;
						U64 colorWeightSize = colorWeightEnd - colorWeightStart;

						if (vertexStart == 0)
						{
							vertexEnd = 0;
							vertexSize = 0;
						}

						if (textureMapStart == 0)
						{
							textureMapEnd = 0;
							textureMapSize = 0;
						}

						if (textureUvStart == 0)
						{
							textureUvEnd = 0;
							textureUvSize = 0;
						}

						if (colorWeightStart == 0)
						{
							colorWeightEnd = 0;
							colorWeightSize = 0;
						}

						PRINT("  | | MdHeader %u:\n", j);
						PRINT("  | | | VertexCount: %u\n", subMesh.Header.VertexCount);
						PRINT("  | | | TextureIndex: %u\n", subMesh.Header.TextureIndex);
						PRINT("  | | | VertexOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", vertexStart, vertexEnd, vertexSize, vertexSize);
						PRINT("  | | | TextureMapOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", textureMapStart, textureMapEnd, textureMapSize, textureMapSize);
						PRINT("  | | | TextureUvOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", textureUvStart, textureUvEnd, textureUvSize, textureUvSize);
						PRINT("  | | | ColorWeightOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", colorWeightStart, colorWeightEnd, colorWeightSize, colorWeightSize);
					}
				}

				break;
			}
			case eModelTypeLevel:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto const& mesh = mScrMeshes[i];

					PRINT("  | MdbHeader %u:\n", i);
					PRINT("  | | MdbId: 0x%08X\n", mesh.Header.MdbId);
					PRINT("  | | MeshType: 0x%08X\n", mesh.Header.MeshType);
					PRINT("  | | MeshId: 0x%04X\n", mesh.Header.MeshId);
					PRINT("  | | SubMeshCount: %u\n", mesh.Header.SubMeshCount);
					PRINT("  | | Transform:\n");
					PRINT("  | | | Position: [%d, %d, %d]\n", mesh.Transform.Position.x, mesh.Transform.Position.y, mesh.Transform.Position.z);
					PRINT("  | | | Rotation: [%d, %d, %d]\n", mesh.Transform.Rotation.x, mesh.Transform.Rotation.y, mesh.Transform.Rotation.z);
					PRINT("  | | | Scale: [%u, %u, %u]\n", mesh.Transform.Scale.x, mesh.Transform.Scale.y, mesh.Transform.Scale.z);

					for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
					{
						auto& subMesh = mesh.SubMeshes[j];

						U64 vertexStart = subMesh.Header.VertexOffset;
						U64 textureMapStart = subMesh.Header.TextureMapOffset;
						U64 textureUvStart = subMesh.Header.TextureUvOffset;
						U64 colorWeightStart = subMesh.Header.ColorWeightOffset;

						U64 vertexEnd = vertexStart + sizeof(ScrVertex) * subMesh.Header.VertexCount;
						U64 textureMapEnd = textureMapStart + sizeof(TextureMap) * subMesh.Header.VertexCount;
						U64 textureUvEnd = textureUvStart + sizeof(ScrUv) * subMesh.Header.VertexCount;
						U64 colorWeightEnd = colorWeightStart + sizeof(ColorWeight) * subMesh.Header.VertexCount;

						U64 vertexSize = vertexEnd - vertexStart;
						U64 textureMapSize = textureMapEnd - textureMapStart;
						U64 textureUvSize = textureUvEnd - textureUvStart;
						U64 colorWeightSize = colorWeightEnd - colorWeightStart;

						if (vertexStart == 0)
						{
							vertexEnd = 0;
							vertexSize = 0;
						}

						if (textureMapStart == 0)
						{
							textureMapEnd = 0;
							textureMapSize = 0;
						}

						if (textureUvStart == 0)
						{
							textureUvEnd = 0;
							textureUvSize = 0;
						}

						if (colorWeightStart == 0)
						{
							colorWeightEnd = 0;
							colorWeightSize = 0;
						}

						PRINT("  | | MdHeader %u:\n", j);
						PRINT("  | | | VertexCount: %u\n", subMesh.Header.VertexCount);
						PRINT("  | | | TextureIndex: %u\n", subMesh.Header.TextureIndex);
						PRINT("  | | | VertexOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", vertexStart, vertexEnd, vertexSize, vertexSize);
						PRINT("  | | | TextureMapOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", textureMapStart, textureMapEnd, textureMapSize, textureMapSize);
						PRINT("  | | | TextureUvOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", textureUvStart, textureUvEnd, textureUvSize, textureUvSize);
						PRINT("  | | | ColorWeightOffset: [0x%016llX .. 0x%016llX] / %llu / 0x%llX\n", colorWeightStart, colorWeightEnd, colorWeightSize, colorWeightSize);
					}
				}

				break;
			}
		}
	}

	void Model::PrintContent(std::function<void(char const*)> Callback)
	{
		static char buffer[1024] = {};

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				if (mScrHeader.MeshCount)
				{
					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						auto const& mesh = mMdMeshes[i];

						PRINT("  Mesh %u:\n", i);

						if (mesh.Header.SubMeshCount)
						{
							for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
							{
								auto& subMesh = mesh.SubMeshes[j];

								PRINT("  | SubMesh %u:\n", j);
								PRINT("  | | Vertices %llu:\n", subMesh.Vertices.size());

								if (subMesh.Vertices.size())
								{
									for (auto const& vertex : subMesh.Vertices)
									{
										PRINT("  | | | [%.3f %.3f %.3f] 0x%04X\n", vertex.X, vertex.Y, vertex.Z, vertex.Connection);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}

								PRINT("  | | TextureMaps %llu:\n", subMesh.TextureMaps.size());

								if (subMesh.TextureMaps.size())
								{
									for (auto const& textureMap : subMesh.TextureMaps)
									{
										PRINT("  | | | [%u %u]\n", textureMap.U, textureMap.V);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}

								PRINT("  | | TextureUVs %llu:\n", subMesh.TextureUvs.size());

								if (subMesh.TextureUvs.size())
								{
									for (auto const& textureUv : subMesh.TextureUvs)
									{
										PRINT("  | | | [%u %u]\n", textureUv.U, textureUv.V);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}

								PRINT("  | | ColorWeights %llu:\n", subMesh.ColorWeights.size());

								if (subMesh.ColorWeights.size())
								{
									for (auto const& colorWeight : subMesh.ColorWeights)
									{
										PRINT("  | | | [%u %u %u %u]\n", colorWeight.R, colorWeight.G, colorWeight.B, colorWeight.A);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}
							}
						}
						else
						{
							PRINT("  | Invalid or empty\n");
						}
					}
				}
				else
				{
					PRINT("  Invalid or empty\n");
				}

				break;
			}
			case eModelTypeLevel:
			{
				if (mScrHeader.MeshCount)
				{
					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						auto const& mesh = mScrMeshes[i];

						PRINT("  Mesh %u:\n", i);

						if (mesh.Header.SubMeshCount)
						{
							for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
							{
								auto& subMesh = mesh.SubMeshes[j];

								PRINT("  | SubMesh %u:\n", j);
								PRINT("  | | Vertices %llu:\n", subMesh.Vertices.size());

								if (subMesh.Vertices.size())
								{
									for (auto const& vertex : subMesh.Vertices)
									{
										PRINT("  | | | [%d %d %d] 0x%04X\n", vertex.X, vertex.Y, vertex.Z, vertex.Connection);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}

								PRINT("  | | TextureMaps %llu:\n", subMesh.TextureMaps.size());

								if (subMesh.TextureMaps.size())
								{
									for (auto const& textureMap : subMesh.TextureMaps)
									{
										PRINT("  | | | [%u %u]\n", textureMap.U, textureMap.V);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}

								PRINT("  | | TextureUVs %llu:\n", subMesh.TextureUvs.size());

								if (subMesh.TextureUvs.size())
								{
									for (auto const& textureUv : subMesh.TextureUvs)
									{
										PRINT("  | | | [%u %u]\n", textureUv.U, textureUv.V);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}

								PRINT("  | | ColorWeights %llu:\n", subMesh.ColorWeights.size());

								if (subMesh.ColorWeights.size())
								{
									for (auto const& colorWeight : subMesh.ColorWeights)
									{
										PRINT("  | | | [%u %u %u %u]\n", colorWeight.R, colorWeight.G, colorWeight.B, colorWeight.A);
									}
								}
								else
								{
									PRINT("  | | | Invalid or empty\n");
								}
							}
						}
						else
						{
							PRINT("  | Invalid or empty\n");
						}
					}
				}
				else
				{
					PRINT("  Invalid or empty\n");
				}

				break;
			}
		}
	}

	bool Model::ConvertIntoProprietaryFormat(fs::path const& FilePath, fs::path const& RulesPath)
	{
		std::string rulesText = "";

		FileUtility::ReadText(RulesPath, rulesText);

		nlohmann::json json = nlohmann::json::parse(rulesText);

		ConversionRules rules = json;

		Assimp::Importer importer = {};

		U32 importFlags = aiProcess_ValidateDataStructure | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData;

		aiScene const* scene = importer.ReadFile(FilePath.string(), importFlags);

		if (scene && (scene->mFlags & ~(AI_SCENE_FLAGS_INCOMPLETE)) && scene->mRootNode)
		{
			return ConvertRoot(scene, rules);
		}

		return false;
	}

	std::string Model::GenerateConversionRules()
	{
		ConversionRules rules = {};

		rules.ScrId = mScrHeader.ScrId;
		rules.FileType = mScrHeader.FileType;
		rules.MeshRules.resize(mScrHeader.MeshCount);

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto const& mesh = mMdMeshes[i];
					auto& meshRule = rules.MeshRules[i];

					meshRule.MdbId = mesh.Header.MdbId;
					meshRule.MeshType = mesh.Header.MeshType;
					meshRule.MeshId = mesh.Header.MeshId;
					meshRule.SubMeshRules.resize(mesh.Header.SubMeshCount);

					for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
					{
						auto const& subMesh = mesh.SubMeshes[j];
						auto& subMeshRule = meshRule.SubMeshRules[j];

						subMeshRule.TextureIndex = subMesh.Header.TextureIndex;
					}
				}

				break;
			}
			case eModelTypeLevel:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto const& mesh = mScrMeshes[i];
					auto& meshRule = rules.MeshRules[i];

					meshRule.MdbId = mesh.Header.MdbId;
					meshRule.MeshType = mesh.Header.MeshType;
					meshRule.MeshId = mesh.Header.MeshId;
					meshRule.SubMeshRules.resize(mesh.Header.SubMeshCount);

					for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
					{
						auto const& subMesh = mesh.SubMeshes[j];
						auto& subMeshRule = meshRule.SubMeshRules[j];

						subMeshRule.TextureIndex = subMesh.Header.TextureIndex;
					}
				}

				break;
			}
		}

		nlohmann::json json = rules;

		return json.dump(4);
	}

	bool Model::ValidateFileAgainstRules(fs::path const& FilePath, fs::path const& RulesPath)
	{
		std::string rulesText = "";

		FileUtility::ReadText(RulesPath, rulesText);

		nlohmann::json json = nlohmann::json::parse(rulesText);

		ConversionRules rules = json;

		Assimp::Importer importer = {};

		U32 importFlags = aiProcess_ValidateDataStructure | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData;

		aiScene const* scene = importer.ReadFile(FilePath.string(), importFlags);

		if (scene && (scene->mFlags & ~(AI_SCENE_FLAGS_INCOMPLETE)) && scene->mRootNode)
		{
			aiNode const* rootNode = scene->mRootNode;

			U32 meshCount = rootNode->mNumChildren;

			if (meshCount != rules.MeshRules.size())
			{
				return false;
			}

			for (U32 i = 0; i < meshCount; i++)
			{
				aiNode const* childNode = rootNode->mChildren[i];

				U32 subMeshCount = childNode->mNumChildren;

				MeshRule const& meshRule = rules.MeshRules[i];

				if (subMeshCount != meshRule.SubMeshRules.size())
				{
					return false;
				}
			}
		}

		return true;
	}

	void Model::ConvertMdVertices(U32 EntityId, std::vector<MdVertex> const& MdVertices, std::vector<TextureMap> const& TextureMaps, std::vector<MdUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights, std::vector<DefaultVertex>& Vertices)
	{
		U16 vertexCount = (U16)MdVertices.size();

		Vertices.resize(vertexCount);

		for (U16 i = 0; i < vertexCount; i++)
		{
			R32 U = (R32)TextureMaps[i].U;
			R32 V = (R32)TextureMaps[i].V;

			if (U < 32767.5F) U += 65535.0F;
			if (V < 32767.5F) V += 65535.0F;

			U /= 65535.0F;
			V /= 65535.0F;

			U *= MAGIC_UV_COEFFICIENT;
			V *= MAGIC_UV_COEFFICIENT;

			R32V3 position = (vertexCount == MdVertices.size()) ? R32V3{ MdVertices[i].X, MdVertices[i].Y, MdVertices[i].Z } : R32V3{};
			R32V2 textureMap = (vertexCount == TextureMaps.size()) ? R32V2{ U, V } : R32V2{};
			R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].U, TextureUvs[i].V } : R32V2{};
			R32V4 colorWeight = (vertexCount == ColorWeights.size()) ? R32V4{ ColorWeights[i].R, ColorWeights[i].G, ColorWeights[i].B, ColorWeights[i].A } / 255.0F : R32V4{};

			Vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight, EntityId };
		}
	}

	void Model::ConvertScrVertices(U32 EntityId, std::vector<ScrVertex> const& ScrVertices, std::vector<TextureMap> const& TextureMaps, std::vector<ScrUv> const& TextureUvs, std::vector<ColorWeight> const& ColorWeights, std::vector<DefaultVertex>& Vertices)
	{
		U16 vertexCount = (U16)ScrVertices.size();

		Vertices.resize(vertexCount);

		for (U16 i = 0; i < vertexCount; i++)
		{
			R32 U = (R32)TextureMaps[i].U;
			R32 V = (R32)TextureMaps[i].V;

			if (U < 32767.5F) U += 65535.0F;
			if (V < 32767.5F) V += 65535.0F;

			U /= 65535.0F;
			V /= 65535.0F;

			U *= MAGIC_UV_COEFFICIENT;
			V *= MAGIC_UV_COEFFICIENT;

			R32V3 position = (vertexCount == ScrVertices.size()) ? R32V3{ ScrVertices[i].X, ScrVertices[i].Y, ScrVertices[i].Z } : R32V3{};
			R32V2 textureMap = (vertexCount == TextureMaps.size()) ? R32V2{ U, V } : R32V2{};
			R32V2 textureUv = (vertexCount == TextureUvs.size()) ? R32V2{ TextureUvs[i].U, TextureUvs[i].V } : R32V2{};
			R32V4 colorWeight = (vertexCount == ColorWeights.size()) ? R32V4{ ColorWeights[i].R, ColorWeights[i].G, ColorWeights[i].B, ColorWeights[i].A } / 255.0F : R32V4{};

			Vertices[i] = DefaultVertex{ position, textureMap, textureUv, colorWeight, EntityId };
		}
	}

	void Model::TriangulateMdVertices(std::vector<MdVertex> const& Vertices, std::vector<U32>& Indices)
	{
		U16 vertexCount = (U16)Vertices.size();

		for (U16 i = 0; i < vertexCount; i++)
		{
			if (Vertices[i].Connection == 0x8000)
			{
				continue;
			}

			if (Vertices[i].Connection)
			{
				Indices.emplace_back(i - 2);
				Indices.emplace_back(i - 1);
				Indices.emplace_back(i - 0);
			}
			else
			{
				Indices.emplace_back(i - 1);
				Indices.emplace_back(i - 2);
				Indices.emplace_back(i - 0);
			}
		}
	}

	void Model::TriangulateScrVertices(std::vector<ScrVertex> const& Vertices, std::vector<U32>& Indices)
	{
		U16 vertexCount = (U16)Vertices.size();

		for (U16 i = 0; i < vertexCount; i++)
		{
			if (Vertices[i].Connection == 0x8000)
			{
				continue;
			}

			if (Vertices[i].Connection)
			{
				Indices.emplace_back(i - 2);
				Indices.emplace_back(i - 1);
				Indices.emplace_back(i - 0);
			}
			else
			{
				Indices.emplace_back(i - 1);
				Indices.emplace_back(i - 2);
				Indices.emplace_back(i - 0);
			}
		}
	}

	void Model::SerializeModel()
	{
		BinaryMediator mediator = { mBytes, mSize };

		mediator.Write<ScrHeader>(mScrHeader);

		WriteTransformOffsets(&mediator);

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto const& mesh = mMdMeshes[i];

					mediator.Write<MdbHeader>(mesh.Header);

					WriteMdSubMeshOffsets(&mediator, mesh);

					for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
					{
						auto const& subMesh = mesh.SubMeshes[j];

						WriteMdSubMesh(&mediator, subMesh);
					}

					mediator.AlignUp(MESH_ALIGNMENT);
				}

				mediator.AlignUp(TRANSFORM_ALIGNMENT);

				WriteMdTransforms(&mediator);

				break;
			}
			case eModelTypeLevel:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto const& mesh = mScrMeshes[i];

					mediator.Write<MdbHeader>(mesh.Header);

					WriteScrSubMeshOffsets(&mediator, mesh);

					for (U16 j = 0; j < mesh.Header.SubMeshCount; j++)
					{
						auto const& subMesh = mesh.SubMeshes[j];

						WriteScrSubMesh(&mediator, subMesh);
					}

					mediator.AlignUp(MESH_ALIGNMENT);
				}

				mediator.AlignUp(TRANSFORM_ALIGNMENT);

				WriteScrTransforms(&mediator);

				break;
			}
		}
	}

	void Model::DeserializeModel()
	{
		BinaryMediator mediator = { mBytes, mSize };

		mScrHeader = mediator.Read<ScrHeader>();

		if (mScrHeader.ScrId == SCR_ID_00726373)
		{
			std::vector<U32> transformOffsets = mediator.Read<U32>(mScrHeader.MeshCount);

			mediator.AlignUp(DEFAULT_ALIGNMENT);

			switch (mScrHeader.FileType)
			{
				case eModelTypeEntity:
				{
					mMdMeshes.resize(mScrHeader.MeshCount);

					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						U64 mdbStart = mediator.GetPosition();

						auto& mesh = mMdMeshes[i];

						mesh.Index = i;
						mesh.Header = mediator.Read<MdbHeader>();

						switch (mesh.Header.MdbId)
						{
							case MDB_ID_0062646D:
							{
								ReadMdSubMesh(&mediator, mesh, mdbStart);

								break;
							}
							default:
							{
								if (mesh.Header.SubMeshCount > 0)
								{
									mesh.Header.SubMeshCount = 0;
								}

								//__debugbreak(); // TODO

								break;
							}
						}

						mediator.AlignUp(DEFAULT_ALIGNMENT);
					}

					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						mediator.SeekAbs(transformOffsets[i]);

						auto& mesh = mMdMeshes[i];

						mesh.Transform = mediator.Read<MdTransform>();
					}

					break;
				}
				case eModelTypeLevel:
				{
					mScrMeshes.resize(mScrHeader.MeshCount);

					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						U64 mdbStart = mediator.GetPosition();

						auto& mesh = mScrMeshes[i];

						mesh.Index = i;
						mesh.Header = mediator.Read<MdbHeader>();

						switch (mesh.Header.MdbId)
						{
							case MDB_ID_0062646D:
							{
								ReadScrSubMesh(&mediator, mesh, mdbStart);

								break;
							}
							default:
							{
								if (mesh.Header.SubMeshCount > 0)
								{
									mesh.Header.SubMeshCount = 0;
								}

								//__debugbreak(); // TODO

								break;
							}
						}

						mediator.AlignUp(DEFAULT_ALIGNMENT);
					}

					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						mediator.SeekAbs(transformOffsets[i]);

						auto& mesh = mScrMeshes[i];

						mesh.Transform = mediator.Read<ScrTransform>();
					}

					break;
				}
			}
		}
	}

	void Model::DeserializeModelAndCollectUniqueNonSerializableInfos(UniqueNonSerializableInfos& UniqueNonSerializableInfos)
	{
		BinaryMediator mediator = { mBytes, mSize };

		mScrHeader = mediator.Read<ScrHeader>();

		UniqueNonSerializableInfos.UniqueScrIds.emplace(mScrHeader.ScrId);
		UniqueNonSerializableInfos.UniqueScrFileTypes.emplace(mScrHeader.FileType);

		if (mScrHeader.ScrId == SCR_ID_00726373)
		{
			std::vector<U32> transformOffsets = mediator.Read<U32>(mScrHeader.MeshCount);

			mediator.AlignUp(DEFAULT_ALIGNMENT);

			switch (mScrHeader.FileType)
			{
				case eModelTypeEntity:
				{
					mMdMeshes.resize(mScrHeader.MeshCount);

					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						U64 mdbStart = mediator.GetPosition();

						auto& mesh = mMdMeshes[i];

						mesh.Index = i;
						mesh.Header = mediator.Read<MdbHeader>();

						UniqueNonSerializableInfos.UniqueMdbIds.emplace(mesh.Header.MdbId);
						UniqueNonSerializableInfos.UniqueMdbMeshTypes.emplace(mesh.Header.MeshType);
						UniqueNonSerializableInfos.UniqueMdbMeshIds.emplace(mesh.Header.MeshId);

						switch (mesh.Header.MdbId)
						{
							case MDB_ID_0062646D:
							{
								ReadMdSubMesh(&mediator, mesh, mdbStart);

								break;
							}
							default:
							{
								if (mesh.Header.SubMeshCount > 0)
								{
									mesh.Header.SubMeshCount = 0;
								}

								break;
							}
						}

						mediator.AlignUp(DEFAULT_ALIGNMENT);
					}

					break;
				}
				case eModelTypeLevel:
				{
					mScrMeshes.resize(mScrHeader.MeshCount);

					for (U32 i = 0; i < mScrHeader.MeshCount; i++)
					{
						U64 mdbStart = mediator.GetPosition();

						auto& mesh = mScrMeshes[i];

						mesh.Index = i;
						mesh.Header = mediator.Read<MdbHeader>();

						UniqueNonSerializableInfos.UniqueMdbIds.emplace(mesh.Header.MdbId);
						UniqueNonSerializableInfos.UniqueMdbMeshTypes.emplace(mesh.Header.MeshType);
						UniqueNonSerializableInfos.UniqueMdbMeshIds.emplace(mesh.Header.MeshId);

						switch (mesh.Header.MdbId)
						{
							case MDB_ID_0062646D:
							{
								ReadScrSubMesh(&mediator, mesh, mdbStart);

								break;
							}
							default:
							{
								if (mesh.Header.SubMeshCount > 0)
								{
									mesh.Header.SubMeshCount = 0;
								}

								break;
							}
						}

						mediator.AlignUp(DEFAULT_ALIGNMENT);
					}

					break;
				}
			}
		}
	}

	bool Model::ConvertRoot(aiScene const* Scene, ConversionRules const& Rules)
	{
		aiNode const* rootNode = Scene->mRootNode;

		U32 meshCount = rootNode->mNumChildren;

		if (meshCount != Rules.MeshRules.size())
		{
			return false;
		}

		mScrHeader.ScrId = Rules.ScrId;
		mScrHeader.FileType = Rules.FileType;
		mScrHeader.MeshCount = meshCount;

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				mMdMeshes.resize(meshCount);

				break;
			}
			case eModelTypeLevel:
			{
				mScrMeshes.resize(meshCount);

				break;
			}
		}

		for (U32 i = 0; i < meshCount; i++)
		{
			aiNode const* childNode = rootNode->mChildren[i];

			MeshRule const& meshRule = Rules.MeshRules[i];

			switch (mScrHeader.FileType)
			{
				case eModelTypeEntity:
				{
					if (!ConvertMdMesh(Scene, childNode, i, meshRule))
					{
						return false;
					}

					break;
				}
				case eModelTypeLevel:
				{
					if (!ConvertScrMesh(Scene, childNode, i, meshRule))
					{
						return false;
					}

					break;
				}
			}
		}

		return true;
	}

	bool Model::ConvertMdMesh(aiScene const* Scene, aiNode const* Node, U32 Index, MeshRule const& Rule)
	{
		U32 subMeshCount = Node->mNumChildren;

		if (subMeshCount != Rule.SubMeshRules.size())
		{
			return false;
		}

		aiVector3D scale = {};
		aiQuaternion rotation = {};
		aiVector3D position = {};

		Node->mTransformation.Decompose(scale, rotation, position); // TODO

		auto& mesh = mMdMeshes[Index];

		mesh.Index = Index;

		mesh.Header.MdbId = Rule.MdbId;
		mesh.Header.MeshType = Rule.MeshType;
		mesh.Header.MeshId = Rule.MeshId;
		mesh.Header.SubMeshCount = (U16)subMeshCount;

		mesh.SubMeshes.resize(subMeshCount);

		mesh.Transform.Scale = R32V3{ 1.0F, 1.0F, 1.0F }; // TODO
		mesh.Transform.Rotation = R32V3{ 0.0F, 0.0F, 0.0F }; // TODO
		mesh.Transform.Position = R32V3{ 0.0F, 0.0F, 0.0F }; // TODO

		for (U32 i = 0; i < subMeshCount; i++)
		{
			aiNode const* childNode = Node->mChildren[i];

			SubMeshRule const& subMeshRule = Rule.SubMeshRules[i];

			if (!ConvertMdSubMesh(Scene, childNode, mesh, i, subMeshRule))
			{
				return false;
			}
		}

		return true;
	}

	bool Model::ConvertScrMesh(aiScene const* Scene, aiNode const* Node, U32 Index, MeshRule const& Rule)
	{
		U32 subMeshCount = Node->mNumChildren;

		if (subMeshCount != Rule.SubMeshRules.size())
		{
			return false;
		}

		aiVector3D scale = {};
		aiQuaternion rotation = {};
		aiVector3D position = {};

		Node->mTransformation.Decompose(scale, rotation, position); // TODO

		auto& mesh = mScrMeshes[Index];

		mesh.Index = Index;

		mesh.Header.MdbId = Rule.MdbId;
		mesh.Header.MeshType = Rule.MeshType;
		mesh.Header.MeshId = Rule.MeshId;
		mesh.Header.SubMeshCount = (U16)subMeshCount;

		mesh.SubMeshes.resize(subMeshCount);

		mesh.Transform.Scale = I16V3{ 4096, 4096, 4096 }; // TODO
		mesh.Transform.Rotation = I16V3{ 0, 0, 0 }; // TODO
		mesh.Transform.Position = I16V3{ 0, 0, 0 }; // TODO

		for (U32 i = 0; i < subMeshCount; i++)
		{
			aiNode const* childNode = Node->mChildren[i];

			SubMeshRule const& subMeshRule = Rule.SubMeshRules[i];

			if (!ConvertScrSubMesh(Scene, childNode, mesh, i, subMeshRule))
			{
				return false;
			}
		}

		return true;
	}

	bool Model::ConvertMdSubMesh(aiScene const* Scene, aiNode const* Node, MdMesh& Mesh, U32 Index, SubMeshRule const& Rule)
	{
		aiMesh const* assimpMesh = Scene->mMeshes[Node->mMeshes[0]];

		if (!assimpMesh->HasPositions())
		{
			return false;
		}

		if (!assimpMesh->HasFaces())
		{
			return false;
		}

		U32 faceCount = assimpMesh->mNumFaces;
		U32 vertexCount = assimpMesh->mNumFaces * 3;

		auto& subMesh = Mesh.SubMeshes[Index];

		subMesh.Index = Index;

		U64 mdStart = 0;
		U64 offset = 0;

		mdStart += sizeof(MdHeader);
		mdStart = ALIGN_UP_BY(mdStart, DEFAULT_ALIGNMENT);

		if (assimpMesh->HasPositions())
		{
			subMesh.Header.VertexOffset = (U32)(mdStart + offset);

			offset += sizeof(MdVertex) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.Header.TextureMapOffset = (U32)(mdStart + offset);

			offset += sizeof(TextureMap) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.Header.TextureUvOffset = (U32)(mdStart + offset);

			offset += sizeof(MdUv) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		if (assimpMesh->HasVertexColors(0))
		{
			subMesh.Header.ColorWeightOffset = (U32)(mdStart + offset);

			offset += sizeof(ColorWeight) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		subMesh.Header.VertexCount = (U16)vertexCount;
		subMesh.Header.TextureIndex = Rule.TextureIndex;

		if (assimpMesh->HasPositions())
		{
			subMesh.Vertices.resize(vertexCount);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.TextureMaps.resize(vertexCount);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.TextureUvs.resize(vertexCount);
		}

		if (assimpMesh->HasVertexColors(0))
		{
			subMesh.ColorWeights.resize(vertexCount);
		}

		for (U32 j = 0, k = 0; j < faceCount; j++, k += 3)
		{
			aiFace assimpFace = assimpMesh->mFaces[j];

			U32 i0 = assimpFace.mIndices[0];
			U32 i1 = assimpFace.mIndices[1];
			U32 i2 = assimpFace.mIndices[2];

			if (assimpMesh->HasPositions())
			{
				aiVector3D v0 = assimpMesh->mVertices[i0];
				aiVector3D v1 = assimpMesh->mVertices[i1];
				aiVector3D v2 = assimpMesh->mVertices[i2];

				subMesh.Vertices[k + 0].X = (R32)v0.x;
				subMesh.Vertices[k + 0].Y = (R32)v0.y;
				subMesh.Vertices[k + 0].Z = (R32)v0.z;
				subMesh.Vertices[k + 0].Connection = 0x8000; // TODO

				subMesh.Vertices[k + 1].X = (R32)v1.x;
				subMesh.Vertices[k + 1].Y = (R32)v1.y;
				subMesh.Vertices[k + 1].Z = (R32)v1.z;
				subMesh.Vertices[k + 1].Connection = 0x8000; // TODO

				subMesh.Vertices[k + 2].X = (R32)v2.x;
				subMesh.Vertices[k + 2].Y = (R32)v2.y;
				subMesh.Vertices[k + 2].Z = (R32)v2.z;
				subMesh.Vertices[k + 2].Connection = 0; // TODO
			}

			if (assimpMesh->HasTextureCoords(0))
			{
				aiVector3D t0 = assimpMesh->mTextureCoords[0][i0];
				aiVector3D t1 = assimpMesh->mTextureCoords[0][i1];
				aiVector3D t2 = assimpMesh->mTextureCoords[0][i2];

				subMesh.TextureMaps[k + 0].U = 0; // TODO
				subMesh.TextureMaps[k + 0].V = 0; // TODO

				subMesh.TextureMaps[k + 1].U = 0; // TODO
				subMesh.TextureMaps[k + 1].V = 0; // TODO

				subMesh.TextureMaps[k + 2].U = 0; // TODO
				subMesh.TextureMaps[k + 2].V = 0; // TODO

				subMesh.TextureUvs[k + 0].U = (U16)t0.x; // TODO
				subMesh.TextureUvs[k + 0].V = (U16)t0.y; // TODO

				subMesh.TextureUvs[k + 1].U = (U16)t1.x; // TODO
				subMesh.TextureUvs[k + 1].V = (U16)t1.y; // TODO

				subMesh.TextureUvs[k + 2].U = (U16)t2.x; // TODO
				subMesh.TextureUvs[k + 2].V = (U16)t2.y; // TODO
			}

			if (assimpMesh->HasVertexColors(0))
			{
				aiColor4D c0 = assimpMesh->mColors[0][i0];
				aiColor4D c1 = assimpMesh->mColors[0][i1];
				aiColor4D c2 = assimpMesh->mColors[0][i2];

				subMesh.ColorWeights[k + 0].R = (U8)(c0.r * 255);
				subMesh.ColorWeights[k + 0].G = (U8)(c0.g * 255);
				subMesh.ColorWeights[k + 0].B = (U8)(c0.b * 255);
				subMesh.ColorWeights[k + 0].A = (U8)(c0.a * 255);

				subMesh.ColorWeights[k + 1].R = (U8)(c1.r * 255);
				subMesh.ColorWeights[k + 1].G = (U8)(c1.g * 255);
				subMesh.ColorWeights[k + 1].B = (U8)(c1.b * 255);
				subMesh.ColorWeights[k + 1].A = (U8)(c1.a * 255);

				subMesh.ColorWeights[k + 2].R = (U8)(c2.r * 255);
				subMesh.ColorWeights[k + 2].G = (U8)(c2.g * 255);
				subMesh.ColorWeights[k + 2].B = (U8)(c2.b * 255);
				subMesh.ColorWeights[k + 2].A = (U8)(c2.a * 255);
			}
		}

		return true;
	}

	bool Model::ConvertScrSubMesh(aiScene const* Scene, aiNode const* Node, ScrMesh& Mesh, U32 Index, SubMeshRule const& Rule)
	{
		aiMesh const* assimpMesh = Scene->mMeshes[Node->mMeshes[Index]];

		if (!assimpMesh->HasPositions())
		{
			return false;
		}

		if (!assimpMesh->HasFaces())
		{
			return false;
		}

		U32 faceCount = assimpMesh->mNumFaces;
		U32 vertexCount = assimpMesh->mNumFaces * 3;

		auto& subMesh = Mesh.SubMeshes[Index];

		subMesh.Index = Index;

		U64 mdStart = 0;
		U64 offset = 0;

		mdStart += sizeof(MdHeader);
		mdStart = ALIGN_UP_BY(mdStart, DEFAULT_ALIGNMENT);

		if (assimpMesh->HasPositions())
		{
			subMesh.Header.VertexOffset = (U32)(mdStart + offset);

			offset += sizeof(ScrVertex) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.Header.TextureMapOffset = (U32)(mdStart + offset);

			offset += sizeof(TextureMap) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.Header.TextureUvOffset = (U32)(mdStart + offset);

			offset += sizeof(ScrUv) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		if (assimpMesh->HasVertexColors(0))
		{
			subMesh.Header.ColorWeightOffset = (U32)(mdStart + offset);

			offset += sizeof(ColorWeight) * vertexCount;
			offset = ALIGN_UP_BY(offset, DEFAULT_ALIGNMENT);
		}

		subMesh.Header.VertexCount = (U16)vertexCount;
		subMesh.Header.TextureIndex = Rule.TextureIndex;

		if (assimpMesh->HasPositions())
		{
			subMesh.Vertices.resize(vertexCount);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.TextureMaps.resize(vertexCount);
		}

		if (assimpMesh->HasTextureCoords(0))
		{
			subMesh.TextureUvs.resize(vertexCount);
		}

		if (assimpMesh->HasVertexColors(0))
		{
			subMesh.ColorWeights.resize(vertexCount);
		}

		for (U32 j = 0, k = 0; j < faceCount; j++, k += 3)
		{
			aiFace assimpFace = assimpMesh->mFaces[j];

			U32 i0 = assimpFace.mIndices[0];
			U32 i1 = assimpFace.mIndices[1];
			U32 i2 = assimpFace.mIndices[2];

			if (assimpMesh->HasPositions())
			{
				aiVector3D v0 = assimpMesh->mVertices[i0];
				aiVector3D v1 = assimpMesh->mVertices[i1];
				aiVector3D v2 = assimpMesh->mVertices[i2];

				subMesh.Vertices[k + 0].X = (I16)v0.x;
				subMesh.Vertices[k + 0].Y = (I16)v0.y;
				subMesh.Vertices[k + 0].Z = (I16)v0.z;
				subMesh.Vertices[k + 0].Connection = 0x8000; // TODO

				subMesh.Vertices[k + 1].X = (I16)v1.x;
				subMesh.Vertices[k + 1].Y = (I16)v1.y;
				subMesh.Vertices[k + 1].Z = (I16)v1.z;
				subMesh.Vertices[k + 1].Connection = 0x8000; // TODO

				subMesh.Vertices[k + 2].X = (I16)v2.x;
				subMesh.Vertices[k + 2].Y = (I16)v2.y;
				subMesh.Vertices[k + 2].Z = (I16)v2.z;
				subMesh.Vertices[k + 2].Connection = 0; // TODO
			}

			if (assimpMesh->HasTextureCoords(0))
			{
				aiVector3D t0 = assimpMesh->mTextureCoords[0][i0];
				aiVector3D t1 = assimpMesh->mTextureCoords[0][i1];
				aiVector3D t2 = assimpMesh->mTextureCoords[0][i2];

				subMesh.TextureMaps[k + 0].U = 0; // TODO
				subMesh.TextureMaps[k + 0].V = 0; // TODO

				subMesh.TextureMaps[k + 1].U = 0; // TODO
				subMesh.TextureMaps[k + 1].V = 0; // TODO

				subMesh.TextureMaps[k + 2].U = 0; // TODO
				subMesh.TextureMaps[k + 2].V = 0; // TODO

				subMesh.TextureUvs[k + 0].U = (U16)t0.x; // TODO
				subMesh.TextureUvs[k + 0].V = (U16)t0.y; // TODO

				subMesh.TextureUvs[k + 1].U = (U16)t1.x; // TODO
				subMesh.TextureUvs[k + 1].V = (U16)t1.y; // TODO

				subMesh.TextureUvs[k + 2].U = (U16)t2.x; // TODO
				subMesh.TextureUvs[k + 2].V = (U16)t2.y; // TODO
			}

			if (assimpMesh->HasVertexColors(0))
			{
				aiColor4D c0 = assimpMesh->mColors[0][i0];
				aiColor4D c1 = assimpMesh->mColors[0][i1];
				aiColor4D c2 = assimpMesh->mColors[0][i2];

				subMesh.ColorWeights[k + 0].R = (U8)(c0.r * 255);
				subMesh.ColorWeights[k + 0].G = (U8)(c0.g * 255);
				subMesh.ColorWeights[k + 0].B = (U8)(c0.b * 255);
				subMesh.ColorWeights[k + 0].A = (U8)(c0.a * 255);

				subMesh.ColorWeights[k + 1].R = (U8)(c1.r * 255);
				subMesh.ColorWeights[k + 1].G = (U8)(c1.g * 255);
				subMesh.ColorWeights[k + 1].B = (U8)(c1.b * 255);
				subMesh.ColorWeights[k + 1].A = (U8)(c1.a * 255);

				subMesh.ColorWeights[k + 2].R = (U8)(c2.r * 255);
				subMesh.ColorWeights[k + 2].G = (U8)(c2.g * 255);
				subMesh.ColorWeights[k + 2].B = (U8)(c2.b * 255);
				subMesh.ColorWeights[k + 2].A = (U8)(c2.a * 255);
			}
		}

		return true;
	}

	void Model::ReadMdSubMesh(BinaryMediator* Mediator, MdMesh& Mesh, U64 MdbStart)
	{
		Mesh.SubMeshes.resize(Mesh.Header.SubMeshCount);

		std::vector<U32> subMeshOffsets = Mediator->Read<U32>(Mesh.Header.SubMeshCount);

		for (U16 j = 0; j < Mesh.Header.SubMeshCount; j++)
		{
			Mediator->SeekAbs(MdbStart + subMeshOffsets[j]);

			U64 mdStart = Mediator->GetPosition();

			auto& subMesh = Mesh.SubMeshes[j];

			subMesh.Index = j;
			subMesh.Header = Mediator->Read<MdHeader>();

			if (subMesh.Header.VertexOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.VertexOffset);
				Mediator->Read<MdVertex>(subMesh.Vertices, subMesh.Header.VertexCount);
			}

			if (subMesh.Header.TextureMapOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.TextureMapOffset);
				Mediator->Read<TextureMap>(subMesh.TextureMaps, subMesh.Header.VertexCount);
			}

			if (subMesh.Header.TextureUvOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.TextureUvOffset);
				Mediator->Read<MdUv>(subMesh.TextureUvs, subMesh.Header.VertexCount);
			}

			if (subMesh.Header.ColorWeightOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.ColorWeightOffset);
				Mediator->Read<ColorWeight>(subMesh.ColorWeights, subMesh.Header.VertexCount);
			}
		}
	}

	void Model::ReadScrSubMesh(BinaryMediator* Mediator, ScrMesh& Mesh, U64 MdbStart)
	{
		Mesh.SubMeshes.resize(Mesh.Header.SubMeshCount);

		std::vector<U32> subMeshOffsets = Mediator->Read<U32>(Mesh.Header.SubMeshCount);

		for (U16 j = 0; j < Mesh.Header.SubMeshCount; j++)
		{
			Mediator->SeekAbs(MdbStart + subMeshOffsets[j]);

			U64 mdStart = Mediator->GetPosition();

			auto& subMesh = Mesh.SubMeshes[j];

			subMesh.Index = j;
			subMesh.Header = Mediator->Read<MdHeader>();

			if (subMesh.Header.VertexOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.VertexOffset);
				Mediator->Read<ScrVertex>(subMesh.Vertices, subMesh.Header.VertexCount);
			}

			if (subMesh.Header.TextureMapOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.TextureMapOffset);
				Mediator->Read<TextureMap>(subMesh.TextureMaps, subMesh.Header.VertexCount);
			}

			if (subMesh.Header.TextureUvOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.TextureUvOffset);
				Mediator->Read<ScrUv>(subMesh.TextureUvs, subMesh.Header.VertexCount);
			}

			if (subMesh.Header.ColorWeightOffset)
			{
				Mediator->SeekAbs(mdStart + subMesh.Header.ColorWeightOffset);
				Mediator->Read<ColorWeight>(subMesh.ColorWeights, subMesh.Header.VertexCount);
			}
		}
	}

	void Model::WriteTransformOffsets(BinaryMediator* Mediator)
	{
		U64 transformOffset = GetFirstTransformOffset();

		for (U32 i = 0; i < mScrHeader.MeshCount; i++)
		{
			Mediator->Write<U32>((U32)transformOffset);

			switch (mScrHeader.FileType)
			{
				case eModelTypeEntity:
				{
					transformOffset += sizeof(MdTransform);

					break;
				}
				case eModelTypeLevel:
				{
					transformOffset += sizeof(ScrTransform);

					break;
				}
			}
		}

		Mediator->AlignUp(TRANSFORM_OFFSET_ALIGNMENT);
	}

	void Model::WriteMdSubMeshOffsets(BinaryMediator* Mediator, MdMesh const& Mesh)
	{
		U16 subMeshCount = (U16)Mesh.SubMeshes.size();
		U64 subMeshOffset = sizeof(MdbHeader);

		subMeshOffset += sizeof(U32) * subMeshCount;
		subMeshOffset = ALIGN_UP_BY(subMeshOffset, DEFAULT_ALIGNMENT);

		for (U16 i = 0; i < subMeshCount; i++)
		{
			auto const& subMesh = Mesh.SubMeshes[i];

			Mediator->Write<U32>((U32)subMeshOffset);

			subMeshOffset += GetMdSubMeshSize(subMesh);
		}

		Mediator->AlignUp(SUBMESH_OFFSET_ALIGNMENT);
	}

	void Model::WriteScrSubMeshOffsets(BinaryMediator* Mediator, ScrMesh const& Mesh)
	{
		U16 subMeshCount = (U16)Mesh.SubMeshes.size();
		U64 subMeshOffset = sizeof(MdbHeader);

		subMeshOffset += sizeof(U32) * subMeshCount;
		subMeshOffset = ALIGN_UP_BY(subMeshOffset, DEFAULT_ALIGNMENT);

		for (U16 i = 0; i < subMeshCount; i++)
		{
			auto const& subMesh = Mesh.SubMeshes[i];

			Mediator->Write<U32>((U32)subMeshOffset);

			subMeshOffset += GetScrSubMeshSize(subMesh);
		}

		Mediator->AlignUp(SUBMESH_OFFSET_ALIGNMENT);
	}

	void Model::WriteMdSubMesh(BinaryMediator* Mediator, MdSubMesh const& SubMesh)
	{
		Mediator->Write<MdHeader>(SubMesh.Header);

		Mediator->AlignUp(DEFAULT_ALIGNMENT);

		if (SubMesh.Header.VertexOffset)
		{
			Mediator->Write<MdVertex>(SubMesh.Vertices);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureMapOffset)
		{
			Mediator->Write<TextureMap>(SubMesh.TextureMaps);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureUvOffset)
		{
			Mediator->Write<MdUv>(SubMesh.TextureUvs);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.ColorWeightOffset)
		{
			Mediator->Write<ColorWeight>(SubMesh.ColorWeights);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		Mediator->AlignUp(SUBMESH_ALIGNMENT);
	}

	void Model::WriteScrSubMesh(BinaryMediator* Mediator, ScrSubMesh const& SubMesh)
	{
		Mediator->Write<MdHeader>(SubMesh.Header);

		Mediator->AlignUp(DEFAULT_ALIGNMENT);

		if (SubMesh.Header.VertexOffset)
		{
			Mediator->Write<ScrVertex>(SubMesh.Vertices);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureMapOffset)
		{
			Mediator->Write<TextureMap>(SubMesh.TextureMaps);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureUvOffset)
		{
			Mediator->Write<ScrUv>(SubMesh.TextureUvs);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.ColorWeightOffset)
		{
			Mediator->Write<ColorWeight>(SubMesh.ColorWeights);
			Mediator->AlignUp(DEFAULT_ALIGNMENT);
		}

		Mediator->AlignUp(SUBMESH_ALIGNMENT);
	}

	void Model::WriteMdTransforms(BinaryMediator* Mediator)
	{
		for (U32 i = 0; i < mScrHeader.MeshCount; i++)
		{
			auto const& mesh = mMdMeshes[i];

			Mediator->Write<MdTransform>(mesh.Transform);
		}
	}

	void Model::WriteScrTransforms(BinaryMediator* Mediator)
	{
		for (U32 i = 0; i < mScrHeader.MeshCount; i++)
		{
			auto const& mesh = mScrMeshes[i];

			Mediator->Write<ScrTransform>(mesh.Transform);
		}
	}

	U64 Model::GetFirstTransformOffset()
	{
		U64 acc = 0;

		acc += sizeof(ScrHeader);

		for (U32 i = 0; i < mScrHeader.MeshCount; i++)
		{
			acc += sizeof(U32);
		}

		acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto& mesh = mMdMeshes[i];

					acc += GetMdMeshSize(mesh);
				}

				break;
			}
			case eModelTypeLevel:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto& mesh = mScrMeshes[i];

					acc += GetScrMeshSize(mesh);
				}

				break;
			}
		}

		acc = ALIGN_UP_BY(acc, TRANSFORM_ALIGNMENT);

		return acc;
	}

	U64 Model::GetMdMeshSize(MdMesh const& Mesh)
	{
		U64 acc = 0;

		acc += sizeof(MdbHeader);
		acc += sizeof(U32) * Mesh.Header.SubMeshCount;
		acc = ALIGN_UP_BY(acc, SUBMESH_OFFSET_ALIGNMENT);

		for (U16 j = 0; j < Mesh.Header.SubMeshCount; j++)
		{
			auto& subMesh = Mesh.SubMeshes[j];

			acc += GetMdSubMeshSize(subMesh);
		}

		acc = ALIGN_UP_BY(acc, MESH_ALIGNMENT);

		return acc;
	}

	U64 Model::GetScrMeshSize(ScrMesh const& Mesh)
	{
		U64 acc = 0;

		acc += sizeof(MdbHeader);
		acc += sizeof(U32) * Mesh.Header.SubMeshCount;
		acc = ALIGN_UP_BY(acc, SUBMESH_OFFSET_ALIGNMENT);

		for (U16 j = 0; j < Mesh.Header.SubMeshCount; j++)
		{
			auto& subMesh = Mesh.SubMeshes[j];

			acc += GetScrSubMeshSize(subMesh);
		}

		acc = ALIGN_UP_BY(acc, MESH_ALIGNMENT);

		return acc;
	}

	U64 Model::GetMdSubMeshSize(MdSubMesh const& SubMesh)
	{
		U64 acc = 0;

		acc += sizeof(MdHeader);
		acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);

		if (SubMesh.Header.VertexOffset)
		{
			acc += sizeof(MdVertex) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureMapOffset)
		{
			acc += sizeof(TextureMap) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureUvOffset)
		{
			acc += sizeof(MdUv) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.ColorWeightOffset)
		{
			acc += sizeof(ColorWeight) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		acc = ALIGN_UP_BY(acc, SUBMESH_ALIGNMENT);

		return acc;
	}

	U64 Model::GetScrSubMeshSize(ScrSubMesh const& SubMesh)
	{
		U64 acc = 0;

		acc += sizeof(MdHeader);
		acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);

		if (SubMesh.Header.VertexOffset)
		{
			acc += sizeof(ScrVertex) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureMapOffset)
		{
			acc += sizeof(TextureMap) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.TextureUvOffset)
		{
			acc += sizeof(ScrUv) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		if (SubMesh.Header.ColorWeightOffset)
		{
			acc += sizeof(ColorWeight) * SubMesh.Header.VertexCount;
			acc = ALIGN_UP_BY(acc, DEFAULT_ALIGNMENT);
		}

		acc = ALIGN_UP_BY(acc, SUBMESH_ALIGNMENT);

		return acc;
	}

	void Model::UpdateSize()
	{
		U64 acc = 0;

		acc += sizeof(ScrHeader);
		acc += sizeof(U32) * mScrHeader.MeshCount;
		acc = ALIGN_UP_BY(acc, TRANSFORM_OFFSET_ALIGNMENT);

		switch (mScrHeader.FileType)
		{
			case eModelTypeEntity:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto& mesh = mMdMeshes[i];

					acc += GetMdMeshSize(mesh);
				}

				acc += sizeof(MdTransform) * mScrHeader.MeshCount;

				break;
			}
			case eModelTypeLevel:
			{
				for (U32 i = 0; i < mScrHeader.MeshCount; i++)
				{
					auto& mesh = mScrMeshes[i];

					acc += GetScrMeshSize(mesh);
				}

				acc += sizeof(ScrTransform) * mScrHeader.MeshCount;

				break;
			}
		}

		acc = ALIGN_UP_BY(acc, EOF_ALIGNMENT);

		mSizePrev = mSize;
		mSize = acc;
	}

	void Model::UpdateByteArray()
	{
		if (mBytes)
		{
			if (mSize != mSizePrev)
			{
				U8* bytes = (U8*)Memory::Alloc(mSize);

				std::memset(bytes, 0, mSize);
				std::memcpy(bytes, mBytes, std::min(mSize, mSizePrev));

				Memory::Free(mBytes);

				mBytes = bytes;
			}
		}
		else
		{
			mBytes = (U8*)Memory::Alloc(mSize);

			std::memset(mBytes, 0, mSize);
		}
	}
}