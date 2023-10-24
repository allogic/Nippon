#pragma once

#include <Vendor/GLM/fwd.hpp>
#include <Vendor/GLM/gtc/quaternion.hpp>

namespace ark
{
	typedef char I8;
	typedef short I16;
	typedef int I32;
	typedef long long I64;

	typedef glm::i8vec2 I8V2;
	typedef glm::i8vec3 I8V3;
	typedef glm::i8vec4 I8V4;

	typedef glm::i16vec2 I16V2;
	typedef glm::i16vec3 I16V3;
	typedef glm::i16vec4 I16V4;

	typedef glm::i32vec2 I32V2;
	typedef glm::i32vec3 I32V3;
	typedef glm::i32vec4 I32V4;

	typedef glm::i64vec2 I64V2;
	typedef glm::i64vec3 I64V3;
	typedef glm::i64vec4 I64V4;

	typedef unsigned char U8;
	typedef unsigned short U16;
	typedef unsigned int U32;
	typedef unsigned long long U64;

	typedef glm::u8vec2 U8V2;
	typedef glm::u8vec3 U8V3;
	typedef glm::u8vec4 U8V4;

	typedef glm::u16vec2 U16V2;
	typedef glm::u16vec3 U16V3;
	typedef glm::u16vec4 U16V4;

	typedef glm::u32vec2 U32V2;
	typedef glm::u32vec3 U32V3;
	typedef glm::u32vec4 U32V4;

	typedef glm::u64vec2 U64V2;
	typedef glm::u64vec3 U64V3;
	typedef glm::u64vec4 U64V4;

	typedef float R32;
	typedef double R64;

	typedef glm::fvec2 R32V2;
	typedef glm::fvec3 R32V3;
	typedef glm::fvec4 R32V4;

	typedef glm::fmat2 R32M2;
	typedef glm::fmat3 R32M3;
	typedef glm::fmat4 R32M4;

	typedef glm::fquat R32Q;

	struct AABB
	{
		R32V3 Min;
		R32V3 Max;
		R32V3 Size;
		R32V3 Center;
	};
}
