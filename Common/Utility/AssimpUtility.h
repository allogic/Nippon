#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Common/Assimp/quaternion.h>
#include <Common/Assimp/vector3.h>
#include <Common/Assimp/color4.h>
#include <Common/Assimp/matrix4x4.h>

namespace Nippon
{
	class AssimpUtility
	{
	public:

		static R32M4 ToGlmMatrix4(aiMatrix4x4 const& Matrix);

		static R32V3 ToGlmVector3(aiVector3D const& Vector);
		static R32V4 ToGlmVector4(aiColor4D const& Color);

		static R32Q ToGlmQuaternion(aiQuaternion const& Orientation);
		static R32V3 ToEulerAngles(aiQuaternion const& Orientation);

		static aiNode const* FindNodeByNameRecursive(aiNode const* Node, std::string const& Name);
	};
}