#ifndef __LUT_INTERPOLATOR_UTILS__
#define __LUT_INTERPOLATOR_UTILS__

namespace Interpolator3D
{

    // --- Clip Function ---
    template <typename T>
    inline T clip(const T& value, const T& min_val, const T& max_val)
    {
        return std::max(min_val, std::min(max_val, value));
    }

} // namespace Interpolator3D


#endif // __LUT_INTERPOLATOR_UTILS__
