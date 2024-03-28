#pragma once

#include <Standard.h>
#include <Types.h>

#include <Archive/Structs/MeshStructs.h>

#include <Common/BinaryMediator.h>

namespace Nippon
{
	class MeshSerializer
	{
	public:

		static std::vector<ScrMesh> ScrMeshesFromBytes(U8 const* Bytes, U64 Size);
		static std::vector<MdMesh> MdMeshesFromBytes(U8 const* Bytes, U64 Size);
	};
}