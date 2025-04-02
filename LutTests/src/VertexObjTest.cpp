#include "gtest/gtest.h"
#include <iostream>
#include "Vertex.hpp"


TEST(VertexTest, VertexF_Ariphmetic)
{
    constexpr Lut::CVertex<float> a(10.f, 20.f, 30.f);
    constexpr Lut::CVertex<float> b(11.f, 21.f, 31.f);
    Lut::CVertex<float> c = a + 1.f;
    EXPECT_EQ(true, b == c);
}

TEST(VertexTest, VertexI_test)
{
    Lut::CVertex<int32_t> a;

    //dummy, let's update it later
    EXPECT_EQ(1, 1);
}
