#pragma once

#include <string>
#include <vector>

#include <Common/Types.h>

#include <Editor/Structs/ModelStructs.h>

namespace ark
{
	class GenericModel
	{
	public:

		enum ModelType
		{
			eModelTypeNone,
			eModelTypeScr,
			eModelTypeMd,
		};

	public:

		inline const auto& GetType() const { return mType; }
		inline const auto& GetName() const { return mName; }
		inline const auto& GetScrMeshes() const { return mScrMeshes; }
		inline const auto& GetMdMeshes() const { return mMdMeshes; }

		inline auto& GetScrMeshes() { return mScrMeshes; }
		inline auto& GetMdMeshes() { return mMdMeshes; }

	public:

		inline void SetType(ModelType Value) { mType = Value; }
		inline void SetName(const std::string& Value) { mName = Value; }
		inline void SetScrMeshes(const std::vector<ScrMesh>& Value) { mScrMeshes = Value; }
		inline void SetMdMeshes(const std::vector<MdMesh>& Value) { mMdMeshes = Value; }

	private:

		ModelType mType = eModelTypeNone;

		std::string mName = "";

		std::vector<ScrMesh> mScrMeshes = {};
		std::vector<MdMesh> mMdMeshes = {};
	};
}
