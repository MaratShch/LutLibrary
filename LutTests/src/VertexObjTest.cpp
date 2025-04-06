#include "gtest/gtest.h"
#include <iostream>
#include "Vertex.hpp"


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
 
    EXPECT_EQ(true, val1 && val2 && val3);
}


TEST(VertexTest, VertexF_MixedAriphmeticOp)
{
    Lut::CVertex<float> a(1.f, 2.f, 3.f);
    Lut::CVertex<int32_t> b(4, 5, 6);

    const auto& c = a + b; // expected 5.f, 7.f, 9.f
    const bool val1 = (c.red() == 5.f && c.green() == 7.f && c.blue() == 9.f ? true : false);
 
    EXPECT_EQ(true, val1);
}


TEST(VertexTest, Vertex_create_test)
{
    Lut::CVertex<int32_t> a = Lut::create_vertex (1, 2, 3);
    Lut::CVertex<float>   b = Lut::create_vertex (1, 2, 3);


    //dummy, let's update it later
    EXPECT_EQ(1, 1);
}
