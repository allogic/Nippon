#pragma once

#include <Vendor/GLM/fwd.hpp>
#include <Vendor/GLM/gtc/quaternion.hpp>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  using I8  = signed char;
  using I16 = signed short;
  using I32 = signed int;
  using I64 = signed long long;

  using I8V2 = glm::i8vec2;
  using I8V3 = glm::i8vec3;
  using I8V4 = glm::i8vec4;

  using I16V2 = glm::i16vec2;
  using I16V3 = glm::i16vec3;
  using I16V4 = glm::i16vec4;

  using U8 = unsigned char;
  using U16 = unsigned short;
  using U32 = unsigned int;
  using U64 = unsigned long long;

  using U8V2 = glm::u8vec2;
  using U8V3 = glm::u8vec3;
  using U8V4 = glm::u8vec4;

  using U16V2 = glm::u16vec2;
  using U16V3 = glm::u16vec3;
  using U16V4 = glm::u16vec4;

  using R32 = float;
  using R64 = double;

  using R32V2 = glm::fvec2;
  using R32V3 = glm::fvec3;
  using R32V4 = glm::fvec4;

  using R32M2 = glm::fmat2;
  using R32M3 = glm::fmat3;
  using R32M4 = glm::fmat4;

  using R32Q = glm::fquat;
}