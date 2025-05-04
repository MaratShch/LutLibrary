#ifndef __LUT_LINEAR_INTERPOLATOR__
#define __LUT_LINEAR_INTERPOLATOR__

#include <array>
#include <cmath>
#include <algorithm>
#include <cstddef>
#include <vector> // Only if returning vector, array preferred

namespace Interpolator3D
{

// --- Clip Function ---
template <typename T>
inline T clip(const T& value, const T& min_val, const T& max_val)
{
    return std::max(min_val, std::min(max_val, value));
}

// --- Helper function for 1D Linear Interpolation (on vectors) ---
template <typename T>
inline LutElement::lutTableRaw<T> interpolate_1d_segment_vec
(
    const LutElement::lutTableRaw<T>& p0, // Assumed size 3
    const LutElement::lutTableRaw<T>& p1, // Assumed size 3
    T t
)
{
    LutElement::lutTableRaw<T> result(3);
    const T one_minus_t = static_cast<T>(1.0) - t;
    result[0] = p0[0] * one_minus_t + p1[0] * t; // R
    result[1] = p0[1] * one_minus_t + p1[1] * t; // G
    result[2] = p0[2] * one_minus_t + p1[2] * t; // B
    return result;
}

// --- Main Linear Interpolation Function (1D over R-axis) ---
template <typename T>
LutElement::lutTableRaw<T> linear_interpolation(
    const LutElement::lutTable3D<T>& lutData,
    T r, T g, T b,
    const LutElement::lutTableRaw<T>& domain_min,
    const LutElement::lutTableRaw<T>& domain_max
) {
//    std::cout << "Using type: " << typeid(T).name() << std::endl;

    const int res_r = static_cast<int>(lutData.size());
    const int res_g = static_cast<int>(lutData[0].size());
    const int res_b = static_cast<int>(lutData[0][0].size());

    // Clip normalized G/B coordinates
    const T g_coord = clip(g, static_cast<T>(0.0), static_cast<T>(1.0));
    const T b_coord = clip(b, static_cast<T>(0.0), static_cast<T>(1.0));

    const int g_idx = clip(
        (res_g > 1) ? static_cast<int>(std::round(g_coord * (res_g - 1))) : 0,
        0, res_g - 1);

    const int b_idx = clip(
        (res_b > 1) ? static_cast<int>(std::round(b_coord * (res_b - 1))) : 0,
        0, res_b - 1);

    // Handle edge case where LUT has only 1 R-point
    if (res_r <= 1) {
        LutElement::lutTableRaw<T> result = lutData[0][g_idx][b_idx];
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
        t = static_cast<T>(0.0); // No interpolation needed
    }

    const auto& p0 = lutData[r_idx0][g_idx][b_idx];
    const auto& p1 = lutData[r_idx1][g_idx][b_idx];

    LutElement::lutTableRaw<T> interpolated_val = interpolate_1d_segment_vec(p0, p1, t);

    LutElement::lutTableRaw<T> clamped_val(3);
    clamped_val[0] = clip(interpolated_val[0], domain_min[0], domain_max[0]);
    clamped_val[1] = clip(interpolated_val[1], domain_min[1], domain_max[1]);
    clamped_val[2] = clip(interpolated_val[2], domain_min[2], domain_max[2]);

    return clamped_val;
}

} // namespace Interpolator3D
#endif // __LUT_LINEAR_INTERPOLATOR__
