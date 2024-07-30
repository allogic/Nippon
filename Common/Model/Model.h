#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Structs/ModelStructs.h>
#include <Common/Structs/PlacementStructs.h>

namespace Nippon
{
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
		void Deserialize(std::vector<U8> const& Bytes);

		void PrintTableOfContent(std::function<void(char const*)> Callback);
		bool ConvertIntoProprietaryFormat(fs::path const& FilePath);

	private:

		void SerializeModel();
		void DeserializeModel();

		bool ConvertRoot(aiScene const* Scene, U32 FileType);
		bool ConvertMdMesh(aiScene const* Scene, aiNode const* Node, U32 Index, U32 MeshType, U16 MeshId);
		bool ConvertScrMesh(aiScene const* Scene, aiNode const* Node, U32 Index, U32 MeshType, U16 MeshId);
		bool ConvertMdSubMesh(aiScene const* Scene, aiNode const* Node, MdMesh& Mesh, U32 Index);
		bool ConvertScrSubMesh(aiScene const* Scene, aiNode const* Node, ScrMesh& Mesh, U32 Index);

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

		// TODO
		U64 mMdbStart = 0;
		U64 mMdStart = 0;

		// TODO
		std::string mName = "";

		// TODO
		Object const* mObjectRef = nullptr;
	};
}