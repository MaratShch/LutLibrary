#include "gtest/gtest.h"
#include <iostream>
#include "Vertex.hpp"


TEST(VertexTest, VertexF_Ariphmetic)
{
    constexpr Lut::CVertex<float> a(10.f, 20.f, 30.f);
    constexpr Lut::CVertex<float> b(11.f, 21.f, 31.f);
    Lut::CVertex<float> c = a + 1.f;

    Lut::Point3D<float> p = c.get_3D_point();

    EXPECT_EQ(true, b == c);
}

TEST(VertexTest, Vertex_create_test)
{
    Lut::CVertex<int32_t> a = Lut::create_vertex (1, 2, 3);
    Lut::CVertex<float>   b = Lut::create_vertex (1, 2, 3);


    //dummy, let's update it later
    EXPECT_EQ(1, 1);
}
