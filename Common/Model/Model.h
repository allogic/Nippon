#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Structs/ModelStructs.h>
#include <Common/Structs/PlacementStructs.h>

#include <Common/Nlohmann/json_fwd.hpp>

namespace Nippon
{
	struct UniqueNonSerializableInfos
	{
		std::set<U32> UniqueScrIds;
		std::set<U32> UniqueScrFileTypes;

		std::set<U32> UniqueMdbIds;
		std::set<U32> UniqueMdbMeshTypes;
		std::set<U16> UniqueMdbMeshIds;
	};

	struct SubMeshRule
	{
		U16 TextureIndex;
	};

	struct MeshRule
	{
		U32 MdbId;
		U32 MeshType;
		U32 MeshId;
		std::vector<SubMeshRule> SubMeshRules;
	};

	struct ConversionRules
	{
		U32 ScrId;
		U32 FileType;
		std::vector<MeshRule> MeshRules;
	};

	void from_json(nlohmann::json const& Json, SubMeshRule& Rule);
	void from_json(nlohmann::json const& Json, MeshRule& Rule);
	void from_json(nlohmann::json const& Json, ConversionRules& Rules);

	class Model
	{
	public:

		Model();
		virtual ~Model();

	public:

		inline auto const& GetScrHeader() const { return mScrHeader; }
		inline auto const& GetName() const { return mName; }
		inline auto const& GetObjectRef() const { return mObjectRef; }
		inline auto const& GetScrMeshes() const { return mScrMeshes; }
		inline auto const& GetMdMeshes() const { return mMdMeshes; }

	public:

		inline void SetName(std::string const& Value) { mName = Value; }
		inline void SetObjectRef(Object const* Value) { mObjectRef = Value; }

	public:

		void SetData(U8 const* Bytes, U64 Size);

	public:

		std::vector<U8> Serialize();

		void Deserialize(U8 const* Bytes, U64 Size);
		void Deserialize(std::vector<U8> const& Bytes);

	public:

		void CollectUniqueNonSerializableInfos(U8 const* Bytes, U64 Size, UniqueNonSerializableInfos& UniqueNonSerializableInfos); // TODO
		void CollectUniqueNonSerializableInfos(std::vector<U8> const& Bytes, UniqueNonSerializableInfos& UniqueNonSerializableInfos); // TODO

	public:

		void PrintTableOfContent(std::function<void(char const*)> Callback);
		void PrintContent(std::function<void(char const*)> Callback);
		bool ConvertIntoProprietaryFormat(fs::path const& FilePath, fs::path const& RulesPath);

	private:

		void SerializeModel();
		void DeserializeModel();

		void DeserializeModelAndCollectUniqueNonSerializableInfos(UniqueNonSerializableInfos& UniqueNonSerializableInfos); // TODO

	private:

		bool ConvertRoot(aiScene const* Scene, ConversionRules const& Rules);
		bool ConvertMdMesh(aiScene const* Scene, aiNode const* Node, U32 Index, MeshRule const& Rule);
		bool ConvertScrMesh(aiScene const* Scene, aiNode const* Node, U32 Index, MeshRule const& Rule);
		bool ConvertMdSubMesh(aiScene const* Scene, aiNode const* Node, MdMesh& Mesh, U32 Index, SubMeshRule const& Rule);
		bool ConvertScrSubMesh(aiScene const* Scene, aiNode const* Node, ScrMesh& Mesh, U32 Index, SubMeshRule const& Rule);

	private:

		void ReadMdSubMesh(BinaryMediator* Mediator, MdMesh& Mesh, U64 MdbStart);
		void ReadScrSubMesh(BinaryMediator* Mediator, ScrMesh& Mesh, U64 MdbStart);

	private:

		void WriteTransformOffsets(BinaryMediator* Mediator);
		void WriteMdSubMeshOffsets(BinaryMediator* Mediator, MdMesh const* Mesh);
		void WriteScrSubMeshOffsets(BinaryMediator* Mediator, ScrMesh const* Mesh);
		void WriteMdSubMesh(BinaryMediator* Mediator, MdSubMesh const* SubMesh);
		void WriteScrSubMesh(BinaryMediator* Mediator, ScrSubMesh const* SubMesh);
		void WriteMdTransforms(BinaryMediator* Mediator);
		void WriteScrTransforms(BinaryMediator* Mediator);

	private:

		U64 GetFirstTransformOffset();
		U64 GetMdSubMeshSize(MdSubMesh const* SubMesh);
		U64 GetScrSubMeshSize(ScrSubMesh const* SubMesh);
		U64 GetSubMeshStructDataOffset(bool IncludeHeader, bool HasData, U64 PrevOffset, U64 StructSize, U64 VertexCount);

	private:

		void UpdateSize();
		void UpdateByteArray();

	private:

		U8* mBytes = nullptr;

		U64 mSize = 0;
		U64 mSizePrev = 0;

		std::vector<ScrMesh> mScrMeshes = {};
		std::vector<MdMesh> mMdMeshes = {};

		ScrHeader mScrHeader = {};

		std::string mName = ""; // TODO

		Object const* mObjectRef = nullptr; // TODO
	};
}