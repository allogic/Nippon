#include <Common/Math.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	R32 Math::Remap(R32 Value, R32 L0, R32 H0, R32 L1, R32 H1)
	{
		return L1 + (Value - L0) * (H1 - L1) / (H0 - L0);
	}
}