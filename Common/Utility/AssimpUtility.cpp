#include <Common/Assimp/scene.h>

#include <Common/Utility/AssimpUtility.h>

namespace Nippon
{
	R32M4 AssimpUtility::ToGlmMatrix4(aiMatrix4x4 const& Matrix)
	{
		R32M4 result = {};

		result[0][0] = Matrix.a1; result[1][0] = Matrix.a2; result[2][0] = Matrix.a3; result[3][0] = Matrix.a4;
		result[0][1] = Matrix.b1; result[1][1] = Matrix.b2; result[2][1] = Matrix.b3; result[3][1] = Matrix.b4;
		result[0][2] = Matrix.c1; result[1][2] = Matrix.c2; result[2][2] = Matrix.c3; result[3][2] = Matrix.c4;
		result[0][3] = Matrix.d1; result[1][3] = Matrix.d2; result[2][3] = Matrix.d3; result[3][3] = Matrix.d4;

		return result;
	}

	R32V3 AssimpUtility::ToGlmVector3(aiVector3D const& Vector)
	{
		return R32V3{ Vector.x, Vector.y, Vector.z };
	}

	R32V4 AssimpUtility::ToGlmVector4(aiColor4D const& Color)
	{
		return R32V4{ Color.r, Color.g, Color.b, Color.a };
	}

	R32Q AssimpUtility::ToGlmQuaternion(aiQuaternion const& Orientation)
	{
		return R32Q{ Orientation.w, Orientation.x, Orientation.y, Orientation.z };
	}

	R32V3 AssimpUtility::ToEulerAngles(aiQuaternion const& Orientation)
	{
		R32 ysqr = Orientation.y * Orientation.y;

		R32 t0 = 2.0F * (Orientation.w * Orientation.x + Orientation.y * Orientation.z);
		R32 t1 = 1.0F - 2.0F * (Orientation.x * Orientation.x + ysqr);
		R32 roll = std::atan2(t0, t1);

		R32 t2 = 2.0F * (Orientation.w * Orientation.y - Orientation.z * Orientation.x);
		t2 = t2 > 1.0F ? 1.0F : t2;
		t2 = t2 < -1.0F ? -1.0F : t2;
		R32 pitch = std::asin(t2);

		R32 t3 = 2.0F * (Orientation.w * Orientation.z + Orientation.x * Orientation.y);
		R32 t4 = 1.0F - 2.0F * (ysqr + Orientation.z * Orientation.z);
		R32 yaw = std::atan2(t3, t4);

		return { roll, pitch, yaw };
	}

	aiNode const* AssimpUtility::FindNodeByNameRecursive(aiNode const* Node, std::string const& Name)
	{
		if (std::strcmp(Name.data(), Node->mName.C_Str()) == 0)
		{
			return Node;
		}

		for (U32 i = 0; i < Node->mNumChildren; i++)
		{
			aiNode const* node = FindNodeByNameRecursive(Node->mChildren[i], Name);

			if (node)
			{
				return node;
			}
		}

		return 0;
	}
}