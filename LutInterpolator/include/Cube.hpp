#ifndef __LUT_LIBRARY_LUT_INTERPOLATOR_SMALL_CUBE__
#define __LUT_LIBRARY_LUT_INTERPOLATOR_SMALL_CUBE__

#include "Vertex.hpp"

namespace Lut
{

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
class CCube3D
{
public:
    using CCube3D_T = T;

private:
    CVertex c000;
    CVertex c001;
    CVertex c010;
    CVertex c011;
    CVertex c100;
    CVertex c101;
    CVertex c110;
    CVertex c111;

}; // CCube3D

};// namespace Lut

#endif // __LUT_LIBRARY_LUT_INTERPOLATOR_SMALL_CUBE__
