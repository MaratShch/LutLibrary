#ifndef __LUT_LIBRARY_LUT_VERTEX_3D__
#define __LUT_LIBRARY_LUT_VERTEX_3D__

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <vector>
#include <array>

namespace Lut
{

template <typename T>
class CVertex
{
public:
	using CVertexT = T;

	// default constructor
    constexpr CVertex (void) noexcept : r{0}, g{0}, b{0} {};
	// copy and move constructors
    constexpr CVertex (const CVertex& other_vertex) noexcept = default;
    constexpr CVertex (CVertex&& other_vertex) noexcept = default;
    // value constructor
	constexpr CVertex (const T& _r_, const T& _g_, const T& _b_) noexcept : r{_r_}, g{_g_}, b{_b_} {};
    constexpr CVertex (const std::tuple<T>& t) noexcept : r{std::get<0>(t)}, g{std::get<1>(t)}, b{std::get<2>(t)} {};
    constexpr CVertex (const std::vector<T>& elem) noexcept : r{elem[0]}, g{elem[1]}, b{elem[2]} {};
    constexpr CVertex (const std::array<T, 3>& elem) noexcept : r{elem[0]}, g{elem[1]}, b{elem[2]} {};

    // class destructor
    ~CVertex(void) noexcept = default;

    /* assignment / copy / move operators */
    CVertex& operator=(const CVertex& other) = default;
    CVertex& operator=(CVertex&& other) noexcept { swap(other); return *this; }

    CVertex& operator=(const std::tuple<T>& t) noexcept { r{std::get<0>(t)}, g{std::get<1>(t)}, b{std::get<2>(t)}, return *this; };
    CVertex& operator=(const std::vector<T>& elem) noexcept { r{elem[0]}, g{elem[1]}, b{elem[2]}, return *this; };

    const T red  (void) const noexcept { return r; }
    const T green(void) const noexcept { return g; }
    const T blue (void) const noexcept { return b; }

    void swap (CVertex& other) noexcept { std::swap(r, other.r), std::swap(g, other.g); std::swap(b, other.b); }

    // inplace floor/ceil
    CVertex& floor(void) noexcept { r = std::floor(r), g = std::floor(g), b = std::floor(b); return *this; }
    CVertex& ceil (void) noexcept { r = std::ceil(r),  g = std::ceil(g),  b = std::ceil(b);  return *this; }
    
    T euclidean_distance (const CVertex& other) const noexcept
    { 
        const T dR{ r - other.r };
        const T dG{ g - other.g };
        const T dB{ b - other.b };
        return std::sqrt(dR * dR + dG * dG + dB * dB); 
    }

    // Prefix increment operator
    CVertex operator ++ () noexcept { return *this; }
    // Postfix increment operator
    CVertex operator ++ (int) noexcept { CVertex tmp(*this); return tmp; }

    // Prefix decrement operator

    // Postfix decrement operator

    // operators +=

    // operators -=

    // operators *=

    // operators /=

    // operator +

    // operator -

    // operator *

    // operator /

    // out stream operator

    // in stream operator 


private:
	T r;
    T g;
    T b;

}; // class Vertex 

   /* logical operators != */

   /* logical operators == */

   /* logical operators < */

   /* logical operators > */

   /* logical operators <= */

   /* logical operators >= */

   // commond defintions and API's 
   using CVertexI = CVertex<int>;
   using CVertexF = CVertex<float>;
   using CVertexD = CVertex<double>;

   // floor/ceil API's
   template <typename T>
   CVertex<T> floor(const CVertex<T>& v) noexcept { return CVertex<T>(v).floor(); }

   template <typename T>
   CVertex<T> ceil (const CVertex<T>& v) noexcept { return CVertex<T>(v).ceil(); }

   template <typename T>
   T euclidean_distance (const CVertex<T>& l, const CVertex<T>& r) noexcept { return r.euclidean_distance(l); }

}; // namespace Lut

#endif // __LUT_LIBRARY_LUT_VERTEX_3D__