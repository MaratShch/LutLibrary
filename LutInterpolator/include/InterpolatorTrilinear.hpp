#ifndef __LUT_TRILINEAR_INTERPOLATOR__
#define __LUT_TRILINEAR_INTERPOLATOR__

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


template <typename T>
LutElement::lutTableRaw<T> trilinear_interpolation
(
    const LutElement::lutTableRaw<T>& lutData,
    T r, T g, T b,
    const LutElement::lutTableRaw<T>& domain_min,
    const LutElement::lutTableRaw<T>& domain_max,
    const LutElement::lutSize& lutSize
)
{
    const int res_r = lutSize.res_r;
    const int res_g = lutSize.res_g;
    const int res_b = lutSize.res_b;

    // --- Input Coordinate Clamping ---
    T x = clip(r, T(0.0), T(1.0));
    T y = clip(g, T(0.0), T(1.0));
    T z = clip(b, T(0.0), T(1.0));

    // --- Handle edge cases where LUT is not fully 3D ---
    if (res_r <= 1 || res_g <= 1 || res_b <= 1)
    {
        // Fallback to bilinear if one dimension is flat
        bool isEffectively3D = (res_r > 1 && res_g > 1 && res_b > 1);
        if (!isEffectively3D)
        {
             return bilinear_interpolation(lutData, r, g, b, domain_min, domain_max, lutSize, true);
        }
    }

    // --- Calculate Indices and Interpolation Weights ---
    T fx = x * static_cast<T>(res_r - 1);
    T fy = y * static_cast<T>(res_g - 1);
    T fz = z * static_cast<T>(res_b - 1);

    int x0 = static_cast<int>(std::floor(fx));
    int y0 = static_cast<int>(std::floor(fy));
    int z0 = static_cast<int>(std::floor(fz));

    // Ensure indices for the 'upper' corner are also within bounds
    int x1 = clip(x0 + 1, 0, res_r - 1);
    int y1 = clip(y0 + 1, 0, res_g - 1);
    int z1 = clip(z0 + 1, 0, res_b - 1);
    // x0 is implicitly clipped by the logic below, but we can be explicit
    x0 = clip(x0, 0, res_r - 1);
    y0 = clip(y0, 0, res_g - 1);
    z0 = clip(z0, 0, res_b - 1);

    // Calculate interpolation weights (fractional part)
    T tx = fx - static_cast<T>(x0);
    T ty = fy - static_cast<T>(y0);
    T tz = fz - static_cast<T>(z0);

    // Helper lambda to fetch a point from the flat LUT
    auto get_point = [&](int r_idx, int g_idx, int b_idx) -> LutElement::lutTableRaw<T> {
        std::size_t tripletIndex = (static_cast<std::size_t>(b_idx) * res_g * res_r) + (static_cast<std::size_t>(g_idx) * res_r) + r_idx;
        std::size_t flatIndexR = tripletIndex * 3;
        return { lutData[flatIndexR], lutData[flatIndexR + 1], lutData[flatIndexR + 2] };
    };

    // Fetch all 8 corner values of the cube
    LutElement::lutTableRaw<T> c000 = get_point(x0, y0, z0);
    LutElement::lutTableRaw<T> c100 = get_point(x1, y0, z0);
    LutElement::lutTableRaw<T> c010 = get_point(x0, y1, z0);
    LutElement::lutTableRaw<T> c110 = get_point(x1, y1, z0);
    LutElement::lutTableRaw<T> c001 = get_point(x0, y0, z1);
    LutElement::lutTableRaw<T> c101 = get_point(x1, y0, z1);
    LutElement::lutTableRaw<T> c011 = get_point(x0, y1, z1);
    LutElement::lutTableRaw<T> c111 = get_point(x1, y1, z1);

    // --- Perform Interpolations ---
    // 1. Interpolate along R (x-axis) for the 4 front/back edges
    LutElement::lutTableRaw<T> c00 = linear_interp_segment(c000, c100, tx);
    LutElement::lutTableRaw<T> c01 = linear_interp_segment(c001, c101, tx);
    LutElement::lutTableRaw<T> c10 = linear_interp_segment(c010, c110, tx);
    LutElement::lutTableRaw<T> c11 = linear_interp_segment(c011, c111, tx);

    // 2. Interpolate along G (y-axis) using the results from step 1
    LutElement::lutTableRaw<T> c0 = linear_interp_segment(c00, c10, ty);
    LutElement::lutTableRaw<T> c1 = linear_interp_segment(c01, c11, ty);

    // 3. Interpolate along B (z-axis) using the results from step 2
    LutElement::lutTableRaw<T> interpolated_val = linear_interp_segment(c0, c1, tz);

    // --- Output Clamping ---
    LutElement::lutTableRaw<T> clamped_val(3);
    clamped_val[0] = clip(interpolated_val[0], domain_min[0], domain_max[0]);
    clamped_val[1] = clip(interpolated_val[1], domain_min[1], domain_max[1]);
    clamped_val[2] = clip(interpolated_val[2], domain_min[2], domain_max[2]);

    return clamped_val;
}

)


#endif // __LUT_TRILINEAR_INTERPOLATOR__