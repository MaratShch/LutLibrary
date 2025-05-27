#ifndef __LUT_LIBRARY_ALGORTIHM_UTILS__
#define __LUT_LIBRARY_ALGORTIHM_UTILS__

namespace LutAlgorithm
{
    
template<typename T> 
inline T getTripletIdx (const T& r, const T& g, const T& b, const T& resR, const T& resG, const T& resB) noexcept
{
    return (((b * resR) * resG) + (g * resR) + r) * 3;	 
}


template<typename T> 
inline T getTripletIdx (const T& r, const T& g, const T& b, const T& lutSize) noexcept
{
    // Calculate index of the triplet itself first
    const T tripletIndex = (b * lutSize * lutSize) + (g * lutSize) + r;
    // Multiply by 3 to get the index of the first component (Red)
    return tripletIndex * static_cast<T>(3);
}


} // namespace LutAlgorithm
#endif // __LUT_LIBRARY_ALGORTIHM_UTILS__