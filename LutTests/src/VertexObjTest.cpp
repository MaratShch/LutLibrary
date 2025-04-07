#include "gtest/gtest.h"
#include <iostream>
#include "Vertex.hpp"

#ifdef PROJECT_GIT_BRANCH
const std::string dbgBranchName = { PROJECT_GIT_BRANCH };
#else
const std::string dbgBranchName = "Not defined";
#endif


TEST(VertexTest, Vertex_create_test)
{
    Lut::CVertex<int32_t> a = Lut::create_vertex (1, 2, 3);
    Lut::CVertex<float>   b = Lut::create_vertex (4.f, 5.f, 6.f);
    Lut::CVertex<double>  c = Lut::create_vertex (0.543, 1.867, 12.598);

    const bool val1 = (a.red() == 1     && a.green() == 2     && a.blue() == 3      ? true : false);
    const bool val2 = (b.red() == 4.f   && b.green() == 5.f   && b.blue() == 6.f    ? true : false);
    const bool val3 = (c.red() == 0.543 && c.green() == 1.867 && c.blue() == 12.598 ? true : false);

    if (!val1 || !val2 || !val3)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, Vertex_SimpleConstructs)
{
    Lut::CVertex<float>   a;
    Lut::CVertex<int32_t> b(1, 2, 3);
    Lut::CVertex<float>   c(1.0f, 2.5f, 3.0f);
    Lut::CVertex<double>  d(11.0, 21.0, 31.0);
    Lut::CVertex<float>   e(c);
    
    // empty constructor result
    const bool val1 = (a.red() == 0.f && a.green() == 0.f && a.blue() == 0.f ? true : false);
    // constructor with values from int32_t
    const bool val2 = (b.red() == 1 && b.green() == 2 && b.blue() == 3 ? true : false);
    // constructor with value from float
    const bool val3 = (c.red() == 1.f && c.green() == 2.5f && c.blue() == 3.0f ? true : false);
    // constructor with value from double
    const bool val4 = (d.red() == 11.0 && d.green() == 21.0 && d.blue() == 31.0 ? true : false);
    // copy constructor
    const bool val5 = (e.red() == 1.f && e.green() == 2.5f && e.blue() == 3.0f ? true : false);
 
    if (!val1 || !val2 || !val3 || !val4 || !val5)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << " val1 = " << val4 <<
                 " val5 = " << val5 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3 && val4 && val5);
}


TEST(VertexTest, Vertex_SimpleConversionConstructs)
{
    Lut::CVertex<float> a(1, 2, 3);
    Lut::CVertex<double>b(11.f, 21.f, 31.f);
    Lut::CVertex<long double>c(10, 12, 0);
    
    // construct by convert from int to float
    const bool val1 = (a.red() == 1.f && a.green() == 2.f && a.blue() == 3.f    ? true : false);
    // construct by convert from float to double
    const bool val2 = (b.red() == 11.0 && b.green() == 21.0 && b.blue() == 31.0 ? true : false);
    // construct by convert from int to double
    const bool val3 = (c.red() == 10.0 && c.green() == 12.0 && c.blue() == 0.0  ? true : false);
 
    if (!val1 || !val2 || !val3)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, Vertex_ConstructFromContainers)
{
    constexpr Lut::Point3D<double> p {0.5, 0.75, 1.25};
    constexpr std::tuple<int32_t, int32_t, int32_t> t {1, 2, 3};
    constexpr std::array<double, 3> a {11.75, 0.57, 9.17};
    std::vector<float> v(3);
    v[0] = 2.5f; v[1] = 1.5f; v[2] = 9.75f;
    
    constexpr Lut::CVertex<int32_t>l1 (t);
    constexpr Lut::CVertex<double> l2 (a);
    Lut::CVertex<float>l3 (v);
    constexpr Lut::CVertex<double> l4 (p);

    const bool val1 = (l1.red() == std::get<0>(t) && l1.green() == std::get<1>(t) && l1.blue() == std::get<2>(t) ? true : false);
    const bool val2 = (l2.red() == a[0]           && l2.green() == a[1]           && l2.blue() == a[2]           ? true : false);
    const bool val3 = (l3.red() == v[0]           && l3.green() == v[1]           && l3.blue() == v[2]           ? true : false);
    const bool val4 = (l4.red() == p.x            && l4.green() == p.y            && l4.blue() == p.z            ? true : false);

    if (!val1 || !val2 || !val3 || !val4)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << " val4 = " << val4 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, Vertex_CopyOperators)
{
    constexpr Lut::Point3D<double> p {0.5, 0.75, 1.25};
    constexpr std::tuple<int32_t, int32_t, int32_t> t {1, 2, 3};
    constexpr std::array<double, 3> a {11.75, 0.57, 9.17};
    std::vector<float> v(3);
    v[0] = 2.5f; v[1] = 1.5f; v[2] = 9.75f;
    
    constexpr Lut::CVertex<int32_t>l1 = t;
    constexpr Lut::CVertex<double> l2 = a;
    Lut::CVertex<float>l3 = v;
    constexpr Lut::CVertex<double> l4 = p;

    const bool val1 = (l1.red() == std::get<0>(t) && l1.green() == std::get<1>(t) && l1.blue() == std::get<2>(t) ? true : false);
    const bool val2 = (l2.red() == a[0]           && l2.green() == a[1]           && l2.blue() == a[2]           ? true : false);
    const bool val3 = (l3.red() == v[0]           && l3.green() == v[1]           && l3.blue() == v[2]           ? true : false);
    const bool val4 = (l4.red() == p.x            && l4.green() == p.y            && l4.blue() == p.z            ? true : false);

    if (!val1 || !val2 || !val3 || !val4)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << " val4 = " << val4 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, Vertex_ConversionConstructFromContainers)
{
    constexpr std::tuple<short, short, short> t {1, 2, 3};
    constexpr std::array<float, 3> a {11.75f, 0.57f, 9.17f};
    std::vector<float> v(3);
    v[0] = 2.5f; v[1] = 1.5f; v[2] = 9.75f;
    
    constexpr Lut::CVertex<double>l1 (t);
    constexpr Lut::CVertex<double>l2 (a);
    Lut::CVertex<long double>l3 (v);

    const bool val1 = (l1.red()   == static_cast<double>(std::get<0>(t)) && 
                       l1.green() == static_cast<double>(std::get<1>(t)) &&
                       l1.blue()  == static_cast<double>(std::get<2>(t)) ? true : false);

    const bool val2 = (l2.red()   == static_cast<double>(a[0]) &&
                       l2.green() == static_cast<double>(a[1]) &&
                       l2.blue()  == static_cast<double>(a[2]) ? true : false);

    const bool val3 = (l3.red()   == static_cast<double>(v[0]) &&
                       l3.green() == static_cast<double>(v[1]) &&
                       l3.blue()  == static_cast<double>(v[2]) ? true : false);

    if (!val1 || !val2 || !val3)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, VertexF_AriphmeticOp1)
{
    Lut::CVertex<float> c;
    Lut::CVertex<float> a(10.f, 20.f, 30.f);
    Lut::CVertex<float> b(11.f, 21.f, 31.f);
    a += 1.f; // expected 11, 21, 31
    const bool val1 = (a.red() == b.red() && a.green() == b.green() && a.blue() == b.blue() ? true : false);
 
    a -= 1.f; // expected 10, 20, 30
    const bool val2 = (a.red() == 10.f && a.green() == 20.f && a.blue() == 30.f ? true : false);

    c = a + b; // expected 21, 41, 61
    const bool val3 = (c.red() == 21.f && c.green() == 41.f && c.blue() == 61.f ? true : false);
 
    if (!val1 || !val2 || !val3)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << std::endl; 
    }

    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, VertexF_AriphmeticOp2)
{
    constexpr float v1 = 1.3f;
    constexpr float v2 = 1.7f;
    constexpr float v3 = 3.6f;

    Lut::CVertex<float> a(v1, v2, v3);
    Lut::CVertex<float> a_copy(a);
    const auto& a_floor = a.floor();     // modify (make floor) in place!
    const auto& a_ceil  = a_copy.ceil(); // modify (make ceil) in place!
    const bool val1 = (std::floor(v1) == a_floor.red() && std::floor(v2) == a_floor.green() && std::floor(v3) == a_floor.blue() ? true : false);
    const bool val2 = (std::ceil (v1) == a_ceil.red()  && std::ceil(v2)  == a_ceil.green()  && std::ceil(v3)  == a_ceil.blue()  ? true : false);

    if (!val1 || !val2)
    {
      std::cout << "floor Vetex(" << v1 << "," << v2 << "," << v3 << "): " << a_floor << " result = " << val1 << std::endl;
      if (!val1)
        std::cout << "expected floor results: " << std::floor(v1) << "," << std::floor(v2) << "," << std::floor(v3) << std::endl;
      std::cout << "ceil  Vetex(" << v1 << "," << v2 << "," << v3 << "): " << a_ceil  << " result = " << val2 << std::endl;
      if (!val2)
        std::cout << "expected ceil  results: " << std::ceil(v1) << "," << std::ceil(v2) << "," << std::ceil(v3) << std::endl;
    }

    EXPECT_EQ(true, val1 && val2);
}


TEST(VertexTest, VertexF_AriphmeticOp3)
{
    constexpr float v1 = 1.3f;
    constexpr float v2 = 1.7f;
    constexpr float v3 = 3.6f;

    Lut::CVertex<float> a(v1, v2, v3);
    const auto& a_floor = Lut::floor(a); // produce new object with floor values from a
    const auto& a_ceil  = Lut::ceil(a);  // produce new object with ceil values from a
    const bool val1 = (std::floor(v1) == a_floor.red() && std::floor(v2) == a_floor.green() && std::floor(v3) == a_floor.blue() ? true : false);
    const bool val2 = (std::ceil (v1) == a_ceil.red()  && std::ceil(v2)  == a_ceil.green()  && std::ceil(v3)  == a_ceil.blue()  ? true : false);

    if (!val1 || !val2)
    {
      std::cout << "floor Vetex(" << v1 << "," << v2 << "," << v3 << "): " << a_floor << " result = " << val1 << std::endl;
      if (!val1)
        std::cout << "expected floor results: " << std::floor(v1) << "," << std::floor(v2) << "," << std::floor(v3) << std::endl;
      std::cout << "ceil  Vetex(" << v1 << "," << v2 << "," << v3 << "): " << a_ceil  << " result = " << val2 << std::endl;
      if (!val2)
        std::cout << "expected ceil  results: " << std::ceil(v1) << "," << std::ceil(v2) << "," << std::ceil(v3) << std::endl;
    }

    EXPECT_EQ(true, val1 && val2);
}


TEST(VertexTest, VertexF_AriphmeticOp4)
{
    constexpr float v1 = 1.3f;
    constexpr float v2 = 1.7f;
    constexpr float v3 = 3.6f;

    Lut::CVertex<float> a(v1, v2, v3);
    const auto& b = a * 2.f; 
    const auto& c = a * 10.f;
    const auto& d = c / 10.f;

    const bool val1 = (b.red() == 2.6f && b.green() == 3.4f && b.blue() == 7.2f ? true : false);
    const bool val2 = (c.red() == 13.f && c.green() == 17.f && c.blue() == 36.f ? true : false);
    const bool val3 = (d.red() == 1.3f && d.green() == 1.7f && d.blue() == 3.6f ? true : false);
 
    if (!val1 || !val2 || !val3)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << std::endl; 
    }

    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, VertexF_Compare)
{
    constexpr float v1 = 1.3f;
    constexpr float v2 = 1.7f;
    constexpr float v3 = 3.6f;
    constexpr float v4 = -0.51f;
    constexpr float v5 = 0.01f;
    constexpr float v6 = 1.11f;

    Lut::CVertex<float> a(v1, v2, v3);
    Lut::CVertex<float> b(v4, v5, v6);
    Lut::CVertex<float> c(a);

    const bool val1 = (a > b  ? true  : false);
    const bool val2 = (b < a  ? true  : false);
    const bool val3 = (a >= b ? true  : false);
    const bool val4 = (a <= b ? false : true);
    const bool val5 = (a == b ? false : true);
    const bool val6 = (a != b ? true  : false);
    const bool val7 = (a == c ? true  : false);
    const bool val8 = (a != c ? false : true);

    if (!val1 || !val2 || !val3 || !val4 || !val5 || !val6 || !val7 || !val8)
    {
       std::cout << "val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << " val1 = " << val4 <<
                 " val5 = " << val5 << " val6 = " << val6 << " val7 = " << val7 << " val8 = " << val8 << std::endl; 
    }
 
    EXPECT_EQ(true, val1 && val2 && val3 && val4 && val5 && val6 && val7);
}



TEST(VertexTest, VertexF_MixedAriphmetic)
{
    Lut::CVertex<float> a(1.f, 2.f, 3.f);
    Lut::CVertex<int32_t> b(4, 5, 6);

    const auto& c = a + b; // expected 5.f, 7.f, 9.f
    const bool val1 = (c.red() == 5.f && c.green() == 7.f && c.blue() == 9.f ? true : false);
 
    Lut::CVertex<double>d(1.0, 2.0, 3.0);
    Lut::CVertex<short int>s(4, 5, 6);
    
    const auto e = d + s; // expceted 5.0, 7.0, 9.0
    const bool val2 = (e.red() == 5.0 && e.green() == 7.0 && e.blue() == 9.0 ? true : false);
 
    const auto f = d + a + d; // expected 3.0, 6.0, 9.0
    const bool val3 = (f.red() == 3.0 && f.green() == 6.0 && f.blue() == 9.0 ? true : false);

    if (!val1 || !val2 || !val3)
    {
       std::cout << "MixedAriphmetic: val1 = " << val1 << " val2 = " << val2 << " val3 = " << val3 << std::endl;
       if (!val1)
         std::cout << "expected = 5.f, 7.f, 9.f computed = " << c.red() << "," << c.green() << "," << c.blue() << std::endl; 
       if (!val2)
         std::cout << "expected = 5.0, 7.0, 9.0 computed = " << e.red() << "," << e.green() << "," << e.blue() << std::endl; 
       if (!val3)
         std::cout << "expected = 3.0, 6.0, 9.0 computed = " << f.red() << "," << f.green() << "," << f.blue() << std::endl; 
    }

    EXPECT_EQ(true, val1 && val2 && val3);
}



int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Branch name: " << dbgBranchName << std::endl;
    return RUN_ALL_TESTS();	
}
