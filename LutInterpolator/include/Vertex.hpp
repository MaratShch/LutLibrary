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
#include <utility>

namespace Lut
{

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
struct Point3D
{
   T x;
   T y;
   T z;

   using Point3D_T = T;
};


template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
class CVertex final
{
public:
    using CVertexT = T;

    // default constructor
    constexpr CVertex (void) noexcept : r{0}, g{0}, b{0} {};
    // copy and move constructors
    explicit constexpr CVertex (const CVertex& other_vertex) noexcept(std::is_nothrow_copy_constructible<T>::value) = default;
    explicit           CVertex (CVertex&& other_vertex) noexcept(std::is_nothrow_move_constructible<T>::value) = default;
    // value constructor
    explicit constexpr CVertex (const T& _r_, const T& _g_, const T& _b_) noexcept : r{_r_}, g{_g_}, b{_b_} {};
    explicit constexpr CVertex (const std::tuple<T, T, T>& t) noexcept : r{std::get<0>(t)}, g{std::get<1>(t)}, b{std::get<2>(t)} {};
    explicit constexpr CVertex (const std::vector<T>& elem)            : r{elem[0]}, g{elem[1]}, b{elem[2]} {};
    explicit constexpr CVertex (const std::array<T,3>& elem)  noexcept : r{elem[0]}, g{elem[1]}, b{elem[2]} {};
    explicit constexpr CVertex (const Point3D<T>& p)          noexcept(std::is_nothrow_copy_constructible<T>::value) : r{p.x}, g{p.y}, b{p.z} {};
    explicit           CVertex (const Point3D<T>&& p)         noexcept(std::is_nothrow_move_constructible<T>::value) : r(std::move(p.x)), g(std::move(p.y)), b(std::move(p.z)) {};

    // conversion constructor
    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex (const CVertex<U>& other) noexcept(std::is_nothrow_copy_constructible<T>::value) : 
                   r{static_cast<T>(other.red())}, g{static_cast<T>(other.green())}, b{static_cast<T>(other.blue())} {};
 
    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
              CVertex (const CVertex<U>&& other) noexcept(std::is_nothrow_move_constructible<T>::value) : 
                   r(std::move(static_cast<T>(other.red()))), g(std::move(static_cast<T>(other.green()))), b(std::move(static_cast<T>(other.blue()))) {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex (const U& _r_, const U& _g_, const U& _b_) noexcept : r{static_cast<T>(_r_)}, g{static_cast<T>(_g_)}, b{static_cast<T>(_b_)} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex (const std::tuple<U, U, U>& t) noexcept : r{static_cast<T>(std::get<0>(t))}, g{static_cast<T>(std::get<1>(t))}, b{static_cast<T>(std::get<2>(t))} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex (const std::vector<U>& elem) : r{static_cast<T>(elem[0])}, g{static_cast<T>(elem[1])}, b{static_cast<T>(elem[2])} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex (const std::array<U,3>& elem) noexcept : r{static_cast<T>(elem[0])}, g{static_cast<T>(elem[1])}, b{static_cast<T>(elem[2])} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    constexpr CVertex (const Point3D<U>& p) noexcept(std::is_nothrow_copy_constructible<T>::value) : r{static_cast<T>(p.x)}, g{static_cast<T>(p.y)}, b{static_cast<T>(p.z)} {};

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
              CVertex (const Point3D<U>&& p) noexcept(std::is_nothrow_move_constructible<T>::value) : r(std::move(static_cast<T>(p.x))), g(std::move(static_cast<T>(p.y))), b(std::move(static_cast<T>(p.z))) {};


    // class destructor
    ~CVertex(void) noexcept = default;


    // assignment / copy / move operators
    CVertex& operator=(const CVertex& other) = default;
    CVertex& operator=(CVertex&& other) noexcept { swap(other); return *this; }

    CVertex& operator=(const std::tuple<T, T, T>& t) noexcept { r = std::get<0>(t), g = std::get<1>(t), b = std::get<2>(t); return *this; };
    CVertex& operator=(const std::vector<T>& elem)            { r = elem[0], g = elem[1], b = elem[2]; return *this; };
    CVertex& operator=(const std::array<T,3>& elem)  noexcept { r = elem[0], g = elem[1], b = elem[2]; return *this; };

    CVertex& operator=(const Point3D<T>& p)          noexcept { r = p.x, g = p.y, b = p.z; return *this; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    CVertex& operator=(const Point3D<U>& p)          noexcept { r = static_cast<T>(p.x), g = static_cast<T>(p.y), b = static_cast<T>(p.z); return *this; }


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

    constexpr Point3D<T> get_3D_point(void) const noexcept {return {r, g, b};}
    void set_3D_point (const Point3D<T>& point) noexcept {r = point.x, g = point.y, b = point.z;}

    // swap operator
    void swap (CVertex& other)    noexcept { std::swap(r, other.r), std::swap(g, other.g); std::swap(b, other.b); }
    void swap (Point3D<T>& other) noexcept { std::swap(r, other.x), std::swap(g, other.y); std::swap(b, other.z); }

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
    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, ClassT> euclidean_distance (const CVertex& other) noexcept
    { 
        const T dR{ r - other.r };
        const T dG{ g - other.g };
        const T dB{ b - other.b };
        return std::sqrt(dR * dR + dG * dG + dB * dB); 
    }

    template <typename ClassT = T>
    std::enable_if_t<std::is_integral<ClassT>::value, ClassT> euclidean_distance (const CVertex& other) noexcept
    { return static_cast<T>(0); }

    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, ClassT> euclidean_distance (const Point3D<ClassT>& other) noexcept
    { 
        const T dR{ r - static_cast<T>(other.x) };
        const T dG{ g - static_cast<T>(other.y) };
        const T dB{ b - static_cast<T>(other.z) };
        return std::sqrt(dR * dR + dG * dG + dB * dB); 
    }

    template <typename ClassT = T>
    std::enable_if_t<std::is_integral<ClassT>::value, ClassT> euclidean_distance (const Point3D<ClassT>& other) noexcept
    { return static_cast<T>(0); }


    template <typename ClassT = T>
    std::enable_if_t<std::is_signed<ClassT>::value, CVertex<T>&> zero_clamp()
    { r=std::max(r, static_cast<ClassT>(0)), g=std::max(g, static_cast<ClassT>(0)), b=std::max(b, static_cast<ClassT>(0)); return *this; }

    template <typename ClassT = T>
    std::enable_if_t<std::is_unsigned<ClassT>::value, CVertex<T>&> zero_clamp()
    { return *this; }

    CVertex& clamp (const T& v_min, const T& v_max) noexcept
    {
        r = std::max(v_min, std::min(r, v_max));
        g = std::max(v_min, std::min(g, v_max));
        b = std::max(v_min, std::min(b, v_max));
        return *this;
    }

    CVertex& abs (void) noexcept {r = std::abs(r), g = std::abs(g), b = std::abs(b); return *this;}

    template <typename ClassT = T>
    std::enable_if_t<std::is_floating_point<ClassT>::value, ClassT> hypot (void)  noexcept { return std::sqrt(r*r + g*g + b*b); }

    constexpr explicit operator bool() const noexcept { return (static_cast<T>(0) != r && static_cast<T>(0) != g && static_cast<T>(0) != b); }
 
    // Prefix increment operator
    CVertex& operator ++ () noexcept { ++r, ++g, ++b; return *this; }
    // Postfix increment operator
    CVertex  operator ++ (int) noexcept { CVertex tmp(*this); operator++(); return tmp; }

    // Prefix decrement operator
    CVertex& operator -- () noexcept { --r, --g, --b; return this->zero_clamp(); }
    // Postfix decrement operator
    CVertex  operator -- (int) noexcept { CVertex tmp(*this); operator--(); return tmp.zero_clamp(); }

    
    // operators +=
    CVertex& operator += (const T& v)          noexcept { r += v, g += v, b += v; return *this; }
    CVertex& operator += (const CVertex& v)    noexcept { r += v.r, g += v.g, b += v.b; return *this; }
    CVertex& operator += (const Point3D<T>& p) noexcept { r += p.x, g += p.y, b += p.z; return *this; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    CVertex& operator += (const U& v) {r += static_cast<T>(v), g += static_cast<T>(v), b += static_cast<T>(v); return *this; }


    // operators -=
    CVertex& operator -= (const T& v)          noexcept { r -= v, g -= v, b -= v; return *this; }
    CVertex& operator -= (const CVertex& v)    noexcept { r -= v.r, g -= v.g, b -= v.b; return *this; }
    CVertex& operator -= (const Point3D<T>& p) noexcept { r -= p.x, g -= p.y, b -= p.z; return *this; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    CVertex& operator -= (const U& v) {r -= static_cast<T>(v), g -= static_cast<T>(v), b -= static_cast<T>(v); return *this; }


    // operators *=
    CVertex& operator *= (const T& v)          noexcept { r *= v, g *= v, b *= v; return *this; }
    CVertex& operator *= (const CVertex& v)    noexcept { r *= v.r, g *= v.g, b *= v.b; return *this; }
    CVertex& operator *= (const Point3D<T>& p) noexcept { r *= p.x, g *= p.y, b *= p.z; return *this; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    CVertex& operator *= (const U& v) {r *= static_cast<T>(v), g *= static_cast<T>(v), b *= static_cast<T>(v); return *this; }


    // operators /=
    CVertex& operator /= (const T& v)
    {
        if (v == static_cast<T>(0)) { throw std::domain_error("Division by zero in CVertex::operator/=(T)"); }
        r /= v; g /= v; b /= v;
        return *this;
    }

    CVertex& operator /= (const CVertex& v)
    {
        // Check each component before division
        if (v.r == static_cast<T>(0) || v.g == static_cast<T>(0) || v.b == static_cast<T>(0)) { throw std::domain_error("Division by zero in CVertex::operator/=(CVertex)"); }
        r /= v.r; g /= v.g; b /= v.b;
        return *this;
    }

    CVertex& operator /= (const Point3D<T>& p)
    {
        // Check each component before division
        if (p.x == static_cast<T>(0) || p.y == static_cast<T>(0) || p.z == static_cast<T>(0)) { throw std::domain_error("Division by zero in CVertex::operator/=(Point3D)"); }
        r /= p.x; g /= p.y; b /= p.z;
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    CVertex& operator /= (const U& v)
    {
        // Check before conversion and division
        if (v == static_cast<U>(0)) { throw std::domain_error("Division by zero in CVertex::operator/=(U)"); }
        const T div = static_cast<T>(v);
        // Re-check after conversion in case T is integer and U was small float
        if (div == static_cast<T>(0)) { throw std::domain_error("Division by zero after conversion in CVertex::operator/=(U)"); }

        r /= div; g /= div; b /= div;
        return *this;
    }


    // operator +
    friend CVertex<T> operator + (CVertex<T> l, const CVertex<T>& r) noexcept { l += r; return l; }
    friend CVertex<T> operator + (CVertex<T> l, const T& r) noexcept { l += r; return l; }
    friend CVertex<T> operator + (const T& l, CVertex<T> r) noexcept { r += l; return r; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator + (CVertex<T> l, const U& r) noexcept { l += r; return l; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator + (const U& l, CVertex<T> r) noexcept { r += l; return r; }



    // operator -
    friend CVertex<T> operator - (CVertex<T> l, const CVertex<T>& r) noexcept { l -= r; return l; }
    friend CVertex<T> operator - (CVertex<T> l, const T& r) noexcept { l -= r; return l; }
    friend CVertex<T> operator - (const T& l, CVertex<T> r) noexcept { r -= l; return r; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator - (CVertex<T> l, const U& r) noexcept { l -= r; return l; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator - (const U& l, CVertex<T> r) noexcept { r -= l; return r; }



    // operator *
    friend CVertex<T> operator * (CVertex<T> l, const CVertex<T>& r) noexcept { l *= r; return l; }
    friend CVertex<T> operator * (CVertex<T> l, const T& r) noexcept { l *= r; return l; }
    friend CVertex<T> operator * (const T& l, CVertex<T> r) noexcept { r *= l; return r; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator * (CVertex<T> l, const U& r) noexcept { l *= r; return l; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator * (const U& l, CVertex<T> r) noexcept { r *= l; return r; }



    // operator /
    friend CVertex<T> operator / (CVertex<T> l, const CVertex<T>& r)  { l /= r; return l; }
    friend CVertex<T> operator / (CVertex<T> l, const T& r)  { l /= r; return l; }
    friend CVertex<T> operator / (const T& l, CVertex<T> r)  { r /= l; return r; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator / (CVertex<T> l, const U& r)  { l /= r; return l; }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    friend CVertex<T> operator / (const U& l, CVertex<T> r)  { r /= l; return r; }



    // out stream operator
    friend std::ostream& operator << (std::ostream& os, const CVertex<T>& v) { os << v.r << ":" << v.g << ":" << v.b; return os; }


private:
    T r;
    T g;
    T b;

}; // class Vertex 


    // operator + (Mixed Types)
    template <
        typename T, typename U, // Need both types for the function template
        typename ResultT = std::common_type_t<T, U>,
        // Constraints check both T and U are arithmetic and not the same
        typename = std::enable_if_t<
                       std::is_arithmetic<T>::value && // Check T
                       std::is_arithmetic<U>::value && // Check U
                       !std::is_same<T, U>::value
                   >
    >
    inline CVertex<ResultT> operator+(const CVertex<T>& l, const CVertex<U>& r)
    {
        return CVertex<ResultT>
        (
            static_cast<ResultT>(l.red())   + static_cast<ResultT>(r.red()),
            static_cast<ResultT>(l.green()) + static_cast<ResultT>(r.green()),
            static_cast<ResultT>(l.blue())  + static_cast<ResultT>(r.blue())
        );
    }

    // operator - (Mixed Types)
    template <
        typename T, typename U, // Need both types for the function template
        typename ResultT = std::common_type_t<T, U>,
        // Constraints check both T and U are arithmetic and not the same
        typename = std::enable_if_t<
                       std::is_arithmetic<T>::value && // Check T
                       std::is_arithmetic<U>::value && // Check U
                       !std::is_same<T, U>::value
                   >
    >
    inline CVertex<ResultT> operator-(const CVertex<T>& l, const CVertex<U>& r)
    {
        return CVertex<ResultT>
        (
            static_cast<ResultT>(l.red())   - static_cast<ResultT>(r.red()),
            static_cast<ResultT>(l.green()) - static_cast<ResultT>(r.green()),
            static_cast<ResultT>(l.blue())  - static_cast<ResultT>(r.blue())
        );
    }

   // operator * (Mixed Types)
    template <
        typename T, typename U, // Need both types for the function template
        typename ResultT = std::common_type_t<T, U>,
        // Constraints check both T and U are arithmetic and not the same
        typename = std::enable_if_t<
                       std::is_arithmetic<T>::value && // Check T
                       std::is_arithmetic<U>::value && // Check U
                       !std::is_same<T, U>::value
                   >
    >
    inline CVertex<ResultT> operator*(const CVertex<T>& l, const CVertex<U>& r)
    {
        return CVertex<ResultT>
        (
            static_cast<ResultT>(l.red())   * static_cast<ResultT>(r.red()),
            static_cast<ResultT>(l.green()) * static_cast<ResultT>(r.green()),
            static_cast<ResultT>(l.blue())  * static_cast<ResultT>(r.blue())
        );
    }

    // operator / (Mixed Types) - Removed noexcept due to potential division by zero
    template <
        typename T, typename U, // Need both types for the function template
        typename ResultT = std::common_type_t<T, U>,
        // Constraints check both T and U are arithmetic and not the same
        typename = std::enable_if_t<
                       std::is_arithmetic<T>::value && // Check T
                       std::is_arithmetic<U>::value && // Check U
                       !std::is_same<T, U>::value
                   >
    >
    inline CVertex<ResultT> operator/(const CVertex<T>& l, const CVertex<U>& r)
    {
        // Perform checks before division
        if (r.red() == static_cast<U>(0) || r.green() == static_cast<U>(0) || r.blue() == static_cast<U>(0))
        {
             throw std::domain_error("Division by zero in operator/(CVertex<T>, CVertex<U>)");
        }

        // Check again after potential cast if ResultT is integer
        ResultT r_red_res   = static_cast<ResultT>(r.red());
        ResultT r_green_res = static_cast<ResultT>(r.green());
        ResultT r_blue_res  = static_cast<ResultT>(r.blue());
        
        if (r_red_res == static_cast<ResultT>(0) || r_green_res == static_cast<ResultT>(0) || r_blue_res == static_cast<ResultT>(0))
        {
             throw std::domain_error("Division by zero after conversion in operator/(CVertex<T>, CVertex<U>)");
        }

        return CVertex<ResultT>
        (
            static_cast<ResultT>(l.red())   / r_red_res,
            static_cast<ResultT>(l.green()) / r_green_res,
            static_cast<ResultT>(l.blue())  / r_blue_res
        );
    }


   // logical operators == 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator == (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return ((l.red() == r.red()) && (l.green() == r.green()) && (l.blue() == r.blue())); }

   // logical operators != 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator != (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return !(l == r); }

   // logical operators < 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator < (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return std::tie(l.red(), l.green(), l.blue()) < std::tie(r.red(), r.green(), r.blue()); }

   // logical operators > 
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator > (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return r < l; } 

   // logical operators <=
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator <= (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return !(r < l); } 

   // logical operators >=
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   inline bool operator >= (const CVertex<T>& l, const CVertex<T>& r) noexcept
   { return !(l < r); } 


   // Common/Generic API's
   // clip
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   CVertex<T> clip(const CVertex<T>& v, const T& min, const T& max) noexcept { return CVertex<T>(v).clamp(min, max); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   CVertex<T> clip(const CVertex<T>& v, const U& min, const U& max) noexcept { return CVertex<T>(v).clamp(static_cast<T>(min), static_cast<T>(max)); }

 
   // floor/ceil/round/trunc API's
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   CVertex<T> floor(const CVertex<T>& v) noexcept { return CVertex<T>(v).floor(); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   CVertex<T> ceil (const CVertex<T>& v) noexcept { return CVertex<T>(v).ceil(); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   CVertex<T> round (const CVertex<T>& v) noexcept { return CVertex<T>(v).round(); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   CVertex<T> trunc (const CVertex<T>& v) noexcept { return CVertex<T>(v).trunc(); }


   // abs
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   CVertex<T> abs (const CVertex<T>& v) noexcept { return CVertex<T>(v).abs(); }

   // hypot
   template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
   T hypot (const CVertex<T>& v) noexcept { return CVertex<T>(v).hypot(); }

   // euclidian disatnce
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   T euclidean_distance (const CVertex<T>& l, const CVertex<T>& r) noexcept { return l.euclidean_distance(r); }

   
   // global create functions
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (void)                               noexcept { return CVertex<T>(); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const CVertex<T>& v)                noexcept { return CVertex<T>(v); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
             CVertex<T> create_vertex (CVertex<T>&& v)                     noexcept { return CVertex<T>(std::move(v)); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const T& r, const T& g, const T& b) noexcept { return CVertex<T>(r, g, b); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const std::tuple<T, T, T>& t)       noexcept { return CVertex<T>(t); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const std::vector<T>& v)            { return CVertex<T>(v); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const std::array<T,3>& a)           noexcept { return CVertex<T>(a); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const Point3D<T>& p)                noexcept { return CVertex<T>(p); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
             CVertex<T> create_vertex (Point3D<T>&& p)                     noexcept { return CVertex<T>(std::move(p)); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const U& r, const U& g, const U& b) noexcept { return CVertex<T>(r, g, b); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const Point3D<U>& p)                noexcept { return CVertex<T>(p); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const std::vector<U>& v)            { return CVertex<T>(v); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const std::tuple<U, U, U>& t)       noexcept { return CVertex<T>(t); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const std::array<U,3>& a)           noexcept { return CVertex<T>(a); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   constexpr CVertex<T> create_vertex (const CVertex<U>& v)                noexcept { return CVertex<T>(v); }

   template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic<U>::value && std::is_arithmetic<T>::value>>
   	     CVertex<T> create_vertex (CVertex<U>&& v)                     noexcept { return CVertex<T>(std::move(v)); }


   // global swap
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   void swap (CVertex<T>& l, CVertex<T>& r) noexcept { l.swap(r); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   void swap (CVertex<T>& l, Point3D<T>& p) noexcept { l.swap(p); }

   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   void swap (Point3D<T>& l, CVertex<T>& p) noexcept { p.swap(l); }


   // zeroing
   template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
   void zero (CVertex<T>& v) {v.zero();}


   // common defintions
   using CVertexI = CVertex<int>;
   using CVertexF = CVertex<float>;
   using CVertexD = CVertex<double>;

}; // namespace Lut

#endif // __LUT_LIBRARY_LUT_VERTEX_3D__
