#pragma once

#include <Standard.h>
#include <Types.h>

#include <Archive/ModelTypes.h>

#include <Archive/Structs/MeshStructs.h>
#include <Archive/Structs/PlacementStructs.h>

namespace Nippon
{
	class Model
	{
	public:

		Model();
		virtual ~Model();

	public:

		inline auto const& GetType() const { return mType; }
		inline auto const& GetName() const { return mName; }
		inline auto const& GetObjectRef() const { return mObjectRef; }
		inline auto const& GetScrMeshes() const { return mScrMeshes; }
		inline auto const& GetMdMeshes() const { return mMdMeshes; }

		inline auto const& GetScrMeshes() { return mScrMeshes; }
		inline auto const& GetMdMeshes() { return mMdMeshes; }

	public:

		inline void SetType(ModelType Value) { mType = Value; }
		inline void SetName(std::string const& Value) { mName = Value; }
		inline void SetObjectRef(Object const* Value) { mObjectRef = Value; }
		inline void SetScrMeshes(std::vector<ScrMesh> const& Value) { mScrMeshes = Value; }
		inline void SetMdMeshes(std::vector<MdMesh> const& Value) { mMdMeshes = Value; }

	private:

		ModelType mType = eModelTypeNone;

		std::string mName = "";

		Object const* mObjectRef = nullptr;

		std::vector<ScrMesh> mScrMeshes = {};
		std::vector<MdMesh> mMdMeshes = {};
	};
}