#include <cstdint>

namespace FastCompute
{
	inline float Floor (const float x) noexcept
	{
		constexpr float magic = 12582912.0f; // 2^23 * 1.5
		float y = x + magic;
		y -= magic;
		return y - (y > x);
	}
	
	inline double Floor (const double x) noexcept
	{
		constexpr double magic = 6755399441055744.0; // 2^52 * 1.5
		double y = x + magic;
		y -= magic;
		return y - (y > x);
	}

}