#ifndef __LUT_LIBRARY_LUT_VERTEX_3D__
#define __LUT_LIBRARY_LUT_VERTEX_3D__

#include <type_traits>
#include <algorithm>
#include <cstddef>
#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>

namespace Lut
{

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
class CVertex
{
public:
    using CVertexT = T;

    // default constructor
    constexpr CVertex (void) noexcept : r{0}, g{0}, b{0} {};
    // copy and move constructors
    explicit constexpr CVertex (const CVertex& other_vertex) noexcept = default;
    explicit constexpr CVertex (CVertex&& other_vertex) noexcept = default;
    // value constructor
    explicit constexpr CVertex (const T& _r_, const T& _g_, const T& _b_) noexcept : r{_r_}, g{_g_}, b{_b_} {};
    explicit constexpr CVertex (const std::tuple<T, T, T>& t) noexcept : r{std::get<0>(t)}, g{std::get<1>(t)}, b{std::get<2>(t)} {};
    explicit constexpr CVertex (const std::vector<T>& elem)            : r{elem[0]}, g{elem[1]}, b{elem[2]} {};
    explicit constexpr CVertex (const std::array<T,3>& elem)  noexcept : r{elem[0]}, g{elem[1]}, b{elem[2]} {};

    // conversion constructor
    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex(const CVertex<U>& other) noexcept : r{static_cast<T>(other.red())}, g{static_cast<T>(other.green())}, b{static_cast<T>(other.blue())} {};
 
    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex(const U& _r_, const U& _g_, const U& _b_) noexcept : r{static_cast<T>(_r_)}, g{static_cast<T>(_g_)}, b{static_cast<T>(_b_)} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex(const std::tuple<U, U, U>& t) noexcept : r{static_cast<T>(std::get<0>(t))}, g{static_cast<T>(std::get<2>(t))}, b{static_cast<T>(std::get<3>(t))} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex(const std::vector<U>& elem) : r{static_cast<T>(elem[0])}, g{static_cast<T>(elem[1])}, b{static_cast<T>(elem[2])} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex(const std::array<U,3>& elem) noexcept : r{static_cast<T>(elem[0])}, g{static_cast<T>(elem[1])}, b{static_cast<T>(elem[2])} {};


    // class destructor
    ~CVertex(void) noexcept = default;


    // assignment / copy / move operators
    CVertex& operator=(const CVertex& other) = default;
    CVertex& operator=(CVertex&& other) noexcept { swap(other); return *this; }

    CVertex& operator=(const std::tuple<T, T, T>& t) noexcept { r = std::get<0>(t), g = std::get<1>(t), b = std::get<2>(t); return *this; };
    CVertex& operator=(const std::vector<T>& elem)            { r = elem[0], g = elem[1], b = elem[2]; return *this; };
    CVertex& operator=(const std::array<T,3>& elem)  noexcept { r = elem[0], g = elem[1], b = elem[2]; return *this; };


    // components get/set API's
    constexpr T red  (void) const noexcept { return r; }
    constexpr T green(void) const noexcept { return g; }
    constexpr T blue (void) const noexcept { return b; }

    void set_red   (const T& R) noexcept { r = R; }
    void set_green (const T& G) noexcept { g = G; }
    void set_blue  (const T& B) noexcept { b = B; }

    void set (const std::vector<T>& v) {r = v.at(0), g = v.at(1), b = v.at(2);}
    std::vector<T> get(void) const {std::vector<T>v(3); v[0] = r, v[1] = g, v[2] = b; return v;}

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    void set (const std::vector<U>& v) {r = static_cast<T>(v.at(0)), g = static_cast<T>(v.at(1)), b = static_cast<T>(v.at(2));}
    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    std::vector<U> get(void) {std::vector<U>v(3); v[0] = static_cast<U>(r), v[1] = static_cast<U>(g), v[2] = static_cast<U>(b); return v;}

 
    // swap operator
    void swap (CVertex& other) noexcept { std::swap(r, other.r), std::swap(g, other.g); std::swap(b, other.b); }


    // zero set API
    CVertex& zero (void) noexcept { r = g = b = static_cast<T>(0); return *this; } 


    // inplace floor
    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, CVertex&> floor() noexcept
    { r = std::floor(r); g = std::floor(g); b = std::floor(b); return *this; }

    template <typename ClassT = T>
    std::enable_if_t<std::is_integral<ClassT>::value, CVertex&> floor() noexcept
    { return *this; }

    // inplace ceil
    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, CVertex&> ceil() noexcept
    { r = std::ceil(r); g = std::ceil(g); b = std::ceil(b); return *this; }

    template <typename ClassT = T>
    std::enable_if_t<std::is_integral<ClassT>::value, CVertex&> ceil() noexcept
    { return *this; }

    // inplace round
    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, CVertex&> round() noexcept
    { r = std::round(r); g = std::round(g); b = std::round(b); return *this; }

    template <typename ClassT = T>
    std::enable_if_t<std::is_integral<ClassT>::value, CVertex&> round() noexcept
    { return *this; }

    // inplace trunc
    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, CVertex&> trunc() noexcept
    { r = std::trunc(r); g = std::trunc(g); b = std::trunc(b); return *this; }

    template <typename ClassT = T>
    std::enable_if_t<std::is_integral<ClassT>::value, CVertex&> trunc() noexcept
    { return *this; }
 
   
    // ariphmetic computations
    T euclidean_distance (const CVertex& other) const noexcept
    { 
        const T dR{ r - other.r };
        const T dG{ g - other.g };
        const T dB{ b - other.b };
        return std::sqrt(dR * dR + dG * dG + dB * dB); 
    }

    CVertex& zero_clamp (void) noexcept { r=std::max(r, static_cast<T>(0)), g=std::max(g, static_cast<T>(0)), b=std::max(b, static_cast<T>(0)); return *this; }

    CVertex& clamp (const T& v_min, const T& v_max) noexcept
    {
        r = std::max(v_min, std::min(r, v_max));
        g = std::max(v_min, std::min(g, v_max));
        b = std::max(v_min, std::min(b, v_max));
        return *this;
    }

    constexpr explicit operator bool() noexcept
    {
       constexpr T zero{0}; 
       return (zero != r && zero != g && zero != b); 
    }
 
    // Prefix increment operator
    CVertex& operator ++ () noexcept { ++r, ++g, ++b; return *this; }
    // Postfix increment operator
    CVertex  operator ++ (int) noexcept { CVertex tmp(*this); operator++(); return tmp; }

    // Prefix decrement operator
    CVertex& operator -- () noexcept { --r, --g, --b; return this->zero_clamp(); }
    // Postfix decrement operator
    CVertex  operator -- (int) noexcept { CVertex tmp(*this); operator--(); return tmp.zero_clamp(); }

    // operators +=
    CVertex& operator += (const T& v) noexcept { r += v, g += v, b += v; return *this; }
    CVertex& operator += (const CVertex& v) noexcept { r += v.r, g += v.g, b += v.b; return *this; }

    // operators -=
    CVertex& operator -= (const T& v) noexcept { r -= v, g -= v, b -= v; return *this; }
    CVertex& operator -= (const CVertex& v) noexcept { r -= v.r, g -= v.g, b -= v.b; return *this; }

    // operators *=
    CVertex& operator *= (const T& v) noexcept { r *= v, g *= v, b *= v; return *this; }
    CVertex& operator *= (const CVertex& v) noexcept { r *= v.r, g *= v.g, b *= v.b; return *this; }

    // operators /=
    CVertex& operator /= (const T& v) noexcept { r /= v, g /= v, b /= v; return *this; }
    CVertex& operator /= (const CVertex& v) noexcept { r /= v.r, g /= v.g, b /= v.b; return *this; }

    // operator +
    friend CVertex<T> operator + (CVertex<T> l, const CVertex<T>& r) noexcept { l += r; return l; }
    friend CVertex<T> operator + (CVertex<T> l, const T& r) noexcept { l += r; return l; }
    friend CVertex<T> operator + (const T& l, CVertex<T> r) noexcept { r += l; return r; }

    // operator -
    friend CVertex<T> operator - (CVertex<T> l, const CVertex<T>& r) noexcept { l -= r; return l; }
    friend CVertex<T> operator - (CVertex<T> l, const T& r) noexcept { l -= r; return l; }
    friend CVertex<T> operator - (const T& l, CVertex<T> r) noexcept { r -= l; return r; }

    // operator *
    friend CVertex<T> operator * (CVertex<T> l, const CVertex<T>& r) noexcept { l *= r; return l; }
    friend CVertex<T> operator * (CVertex<T> l, const T& r) noexcept { l *= r; return l; }
    friend CVertex<T> operator * (const T& l, CVertex<T> r) noexcept { r *= l; return r; }

    // operator /
    friend CVertex<T> operator / (CVertex<T> l, const CVertex<T>& r) noexcept { l /= r; return l; }
    friend CVertex<T> operator / (CVertex<T> l, const T& r) noexcept { l /= r; return l; }
    friend CVertex<T> operator / (const T& l, CVertex<T> r) noexcept { r /= l; return r; }


    // out stream operator
    friend std::ostream& operator << (std::ostream& os, const CVertex<T>& v) { os << v.r << ":" << v.g << ":" << v.b; return os; }


private:
    T r;
    T g;
    T b;

}; // class Vertex 

   // logical operators == 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator == (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return (l.red == r.red && l.green == r.green && l.blue == r.blue); }

   // logical operators != 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator != (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return !(l == r); }

   // logical operators < 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator < (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return (l.red < r.red && l.green < r.green && l.blue < r.blue); }

   // logical operators > 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator > (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return (l.red > r.red && l.green > r.green && l.blue > r.blue); }

   // logical operators <=
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator <= (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return (l.red <= r.red && l.green <= r.green && l.blue <= r.blue); }

   // logical operators >=
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator >= (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return (l.red >= r.red && l.green >= r.green && l.blue >= r.blue); }

   // commond defintions and API's 
   using CVertexI = CVertex<int>;
   using CVertexF = CVertex<float>;
   using CVertexD = CVertex<double>;

   // floor/ceil/round/trunc API's
   template <typename T>
   CVertex<T> floor(const CVertex<T>& v) noexcept { return CVertex<T>(v).floor(); }

   template <typename T>
   CVertex<T> ceil (const CVertex<T>& v) noexcept { return CVertex<T>(v).ceil(); }

   template <typename T>
   CVertex<T> round (const CVertex<T>& v) noexcept { return CVertex<T>(v).round(); }

   template <typename T>
   CVertex<T> trunc (const CVertex<T>& v) noexcept { return CVertex<T>(v).trunc(); }

   template <typename T>
   T euclidean_distance (const CVertex<T>& l, const CVertex<T>& r) noexcept { return r.euclidean_distance(l); }

}; // namespace Lut

#endif // __LUT_LIBRARY_LUT_VERTEX_3D__
