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

  using U8  = unsigned char;
  using U16 = unsigned short;
  using U32 = unsigned int;
  using U64 = unsigned long long;

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