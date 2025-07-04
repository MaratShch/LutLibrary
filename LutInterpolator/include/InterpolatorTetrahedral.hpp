#ifndef __LUT_TETRAHEDRAL_INTERPOLATOR__
#define __LUT_TETRAHEDRAL_INTERPOLATOR__

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
LutElement::lutTableRaw<T> tetrahedral_interpolation
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

    // Fallback if not fully 3D
    if (res_r <= 1 || res_g <= 1 || res_b <= 1)
	{
        return trilinear_interpolation(lutData, r, g, b, domain_min, domain_max, lutSize);
    }

    // --- Calculate Indices and Fractional Weights ---
    T fx = x * static_cast<T>(res_r - 1);
    T fy = y * static_cast<T>(res_g - 1);
    T fz = z * static_cast<T>(res_b - 1);

    int x0 = static_cast<int>(std::floor(fx));
    int y0 = static_cast<int>(std::floor(fy));
    int z0 = static_cast<int>(std::floor(fz));

    int x1 = clip(x0 + 1, 0, res_r - 1);
    int y1 = clip(y0 + 1, 0, res_g - 1);
    int z1 = clip(z0 + 1, 0, res_b - 1);
    x0 = clip(x0, 0, res_r - 1);
    y0 = clip(y0, 0, res_g - 1);
    z0 = clip(z0, 0, res_b - 1);

    T tx = fx - static_cast<T>(x0);
    T ty = fy - static_cast<T>(y0);
    T tz = fz - static_cast<T>(z0);

    // Helper lambda to fetch a point
    auto get_point = [&](int r_idx, int g_idx, int b_idx) -> std::array<T, 3>
	{
        std::size_t tripletIndex = (static_cast<std::size_t>(b_idx) * res_g * res_r) + (static_cast<std::size_t>(g_idx) * res_r) + r_idx;
        std::size_t flatIndexR = tripletIndex * 3;
        return { lutData[flatIndexR], lutData[flatIndexR + 1], lutData[flatIndexR + 2] };
    };

    // Fetch all 8 corner values of the cube
    std::array<T, 3> c000 = get_point(x0, y0, z0); // Black corner
    std::array<T, 3> c100 = get_point(x1, y0, z0); // Red corner
    std::array<T, 3> c010 = get_point(x0, y1, z0); // Green corner
    std::array<T, 3> c001 = get_point(x0, y0, z1); // Blue corner
    std::array<T, 3> c110 = get_point(x1, y1, z0); // Yellow corner
    std::array<T, 3> c101 = get_point(x1, y0, z1); // Magenta corner
    std::array<T, 3> c011 = get_point(x0, y1, z1); // Cyan corner
    std::array<T, 3> c111 = get_point(x1, y1, z1); // White corner

    LutElement::lutTableRaw<T> interpolated_val(3);
    
    // Determine which tetrahedron the point is in and interpolate
    if (tx > ty)
	{
        if (ty > tz) { // R > G > B
            interpolated_val[0] = (c000[0] + (c100[0]-c000[0])*tx + (c010[0]-c000[0])*ty + (c001[0]-c000[0])*tz);
            interpolated_val[1] = (c000[1] + (c100[1]-c000[1])*tx + (c010[1]-c000[1])*ty + (c001[1]-c000[1])*tz);
            interpolated_val[2] = (c000[2] + (c100[2]-c000[2])*tx + (c010[2]-c000[2])*ty + (c001[2]-c000[2])*tz);
        } else if (tx > tz) { // R > B > G
            interpolated_val[0] = (c000[0] + (c100[0]-c000[0])*tx + (c001[0]-c000[0])*tz + (c010[0]-c000[0])*ty);
            interpolated_val[1] = (c000[1] + (c100[1]-c000[1])*tx + (c001[1]-c000[1])*tz + (c010[1]-c000[1])*ty);
            interpolated_val[2] = (c000[2] + (c100[2]-c000[2])*tx + (c001[2]-c000[2])*tz + (c010[2]-c000[2])*ty);
        } else { // B > R > G
            interpolated_val[0] = (c001[0] + (c101[0]-c001[0])*tx + (c011[0]-c001[0])*ty + (c111[0]-c101[0]-c011[0]+c001[0])*tz);
            interpolated_val[1] = (c001[1] + (c101[1]-c001[1])*tx + (c011[1]-c001[1])*ty + (c111[1]-c101[1]-c011[1]+c001[1])*tz);
            interpolated_val[2] = (c001[2] + (c101[2]-c001[2])*tx + (c011[2]-c001[2])*ty + (c111[2]-c101[2]-c011[2]+c001[2])*tz);
        }
    } 
	else
	{ // G >= R
        if (tz > ty) { // B > G > R
            interpolated_val[0] = (c001[0] + (c011[0]-c001[0])*ty + (c101[0]-c001[0])*tx + (c111[0]-c011[0]-c101[0]+c001[0])*tz);
            interpolated_val[1] = (c001[1] + (c011[1]-c001[1])*ty + (c101[1]-c001[1])*tx + (c111[1]-c011[1]-c101[1]+c001[1])*tz);
            interpolated_val[2] = (c001[2] + (c011[2]-c001[2])*ty + (c101[2]-c001[2])*tx + (c111[2]-c011[2]-c101[2]+c001[2])*tz);
        } else if (tz > tx) { // G > B > R
            interpolated_val[0] = (c010[0] + (c110[0]-c010[0])*tx + (c011[0]-c010[0])*tz + (c111[0]-c110[0]-c011[0]+c010[0])*ty);
            interpolated_val[1] = (c010[1] + (c110[1]-c010[1])*tx + (c011[1]-c010[1])*tz + (c111[1]-c110[1]-c011[1]+c010[1])*ty);
            interpolated_val[2] = (c010[2] + (c110[2]-c010[2])*tx + (c011[2]-c010[2])*tz + (c111[2]-c110[2]-c011[2]+c010[2])*ty);
        } else { // G > R > B
            interpolated_val[0] = (c010[0] + (c110[0]-c010[0])*tx + (c001[0]-c000[0])*tz + (c111[0]-c110[0]-c101[0]+c100[0])*ty);
            interpolated_val[1] = (c010[1] + (c110[1]-c010[1])*tx + (c001[1]-c000[1])*tz + (c111[1]-c110[1]-c101[1]+c100[1])*ty);
            interpolated_val[2] = (c010[2] + (c110[2]-c010[2])*tx + (c001[2]-c000[2])*tz + (c111[2]-c110[2]-c101[2]+c100[2])*ty);
        }
    }
    
    // --- Output Clamping ---
    LutElement::lutTableRaw<T> clamped_val(3);
    clamped_val[0] = clip(interpolated_val[0], domain_min[0], domain_max[0]);
    clamped_val[1] = clip(interpolated_val[1], domain_min[1], domain_max[1]);
    clamped_val[2] = clip(interpolated_val[2], domain_min[2], domain_max[2]);

    return clamped_val;
}

)


#endif // __LUT_TETRAHEDRAL_INTERPOLATOR__