#ifndef __LUT_LINEAR_INTERPOLATOR__
#define __LUT_LINEAR_INTERPOLATOR__

#include <array>
#include <cmath>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "InterpolatorUtils.hpp"
#include "lutElement.h"
#include "lutErrors.h"

namespace Interpolator
{

// --- Helper function for 1D Linear Interpolation (on vectors) ---
template <typename T>
inline LutElement::lutTableRaw<T> interpolate_1d_segment_vec
(
    const LutElement::lutTableRaw<T>& p0,
    const LutElement::lutTableRaw<T>& p1,
    T t
)
{
    LutElement::lutTableRaw<T> result(3);
    const T one_minus_t = static_cast<T>(1.0) - t;
    result[0] = p0[0] * one_minus_t + p1[0] * t;
    result[1] = p0[1] * one_minus_t + p1[1] * t;
    result[2] = p0[2] * one_minus_t + p1[2] * t;
    return result;
}


// --- Extract RGB triplet from flat LUT vector ---
template <typename T>
inline LutElement::lutTableRaw<T> getTriplet(const LutElement::lutTableRaw<T>& lutData, int r, int g, int b, int lutSize)
{
    const int base = LutAlgorithm::getTripletIdx(r, g, b, lutSize);
    return { lutData[base + 0], lutData[base + 1], lutData[base + 2] };
}

// --- Main Linear Interpolation Function (1D over R-axis) ---
template <typename T>
LutElement::lutTableRaw<T> linear_interpolation
(
    const LutElement::lutTableRaw<T>& lutData,
    T r, T g, T b,
    const LutElement::lutTableRaw<T>& domain_min,
    const LutElement::lutTableRaw<T>& domain_max
) {
    const int lutSize = static_cast<int>(std::round(std::cbrt(lutData.size() / 3.0))); // Cube root of triplet count
    const int res_r = lutSize;
    const int res_g = lutSize;
    const int res_b = lutSize;

    const T g_coord = clip(g, static_cast<T>(0.0), static_cast<T>(1.0));
    const T b_coord = clip(b, static_cast<T>(0.0), static_cast<T>(1.0));

    const int g_idx = clip((res_g > 1) ? static_cast<int>(std::round(g_coord * (res_g - 1))) : 0, 0, res_g - 1);
    const int b_idx = clip((res_b > 1) ? static_cast<int>(std::round(b_coord * (res_b - 1))) : 0, 0, res_b - 1);

    if (res_r <= 1) {
        LutElement::lutTableRaw<T> result = getTriplet(lutData, 0, g_idx, b_idx, lutSize);
        result[0] = clip(result[0], domain_min[0], domain_max[0]);
        result[1] = clip(result[1], domain_min[1], domain_max[1]);
        result[2] = clip(result[2], domain_min[2], domain_max[2]);
        return result;
    }

    const T r_coord = clip(r, static_cast<T>(0.0), static_cast<T>(1.0));
    const T f_index = r_coord * static_cast<T>(res_r - 1);

    int r_idx0 = static_cast<int>(std::floor(f_index));
    int r_idx1 = static_cast<int>(std::ceil(f_index));

    r_idx0 = clip(r_idx0, 0, res_r - 1);
    r_idx1 = clip(r_idx1, 0, res_r - 1);

    T t = f_index - static_cast<T>(r_idx0);
    if (r_idx0 == r_idx1) {
        t = static_cast<T>(0.0);
    }

    const auto p0 = getTriplet(lutData, r_idx0, g_idx, b_idx, lutSize);
    const auto p1 = getTriplet(lutData, r_idx1, g_idx, b_idx, lutSize);

    LutElement::lutTableRaw<T> interpolated_val = interpolate_1d_segment_vec(p0, p1, t);

    LutElement::lutTableRaw<T> clamped_val(3);
    clamped_val[0] = clip(interpolated_val[0], domain_min[0], domain_max[0]);
    clamped_val[1] = clip(interpolated_val[1], domain_min[1], domain_max[1]);
    clamped_val[2] = clip(interpolated_val[2], domain_min[2], domain_max[2]);

    return clamped_val;
}

} // namespace Interpolator3D
#endif // __LUT_LINEAR_INTERPOLATOR__
