#ifndef __LUT_BILINEAR_INTERPOLATOR__
#define __LUT_BILINEAR_INTERPOLATOR__

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
    LutElement::lutTableRaw<T> bilinear_interpolation
    (
        const LutElement::lutTableRaw<T>& lutData,
        T r, T g, T b,
        const LutElement::lutTableRaw<T>& domain_min,
        const LutElement::lutTableRaw<T>& domain_max,
        const LutElement::lutSize& lutSize
        bool is3D = true
    )
    {
        constexpr T zero = static_cast<T>(0);
        constexpr T one  = static_cast<T>(1);

        LutElement::lutTableRaw<T> clamped_val(3);
        
        return clamped_val;
    }

} // namespace Interpolator

#endif // __LUT_BILINEAR_INTERPOLATOR__