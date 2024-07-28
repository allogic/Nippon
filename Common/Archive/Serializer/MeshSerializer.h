#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Archive/Structs/MeshStructs.h>

#include <Common/Misc/BinaryMediator.h>

namespace Nippon
{
	class MeshSerializer
	{
	public:

		static std::vector<ScrMesh> ScrMeshesFromBytes(U8 const* Bytes, U64 Size);
		static std::vector<MdMesh> MdMeshesFromBytes(U8 const* Bytes, U64 Size);
	};
}