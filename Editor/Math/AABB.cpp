#include <Math/AABB.h>

namespace Nippon
{
	AABB::AABB()
	{

	}

	AABB::AABB(R32V3 const& Min, R32V3 const& Max)
	{
		mMin = Min;
		mMax = Max;

		mSize = glm::abs(mMin - mMax);
		mCenter = mMin + (mSize / 2.0F);
	}

	AABB::~AABB()
	{

	}
}