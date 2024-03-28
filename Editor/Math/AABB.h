#pragma once

#include <Types.h>

namespace Nippon
{
	struct AABB
	{
	public:

		AABB();
		AABB(R32V3 const& Min, R32V3 const& Max);
		virtual ~AABB();

	public:

		inline auto const& GetMin() const { return mMin; }
		inline auto const& GetMax() const { return mMax; }
		inline auto const& GetSize() const { return mSize; }
		inline auto const& GetCenter() const { return mCenter; }

	private:

		R32V3 mMin = {};
		R32V3 mMax = {};

		R32V3 mSize = {};
		R32V3 mCenter = {};
	};
}