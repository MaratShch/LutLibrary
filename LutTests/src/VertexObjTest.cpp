#include "gtest/gtest.h"
#include <iostream>
#include "Vertex.hpp"


TEST(VertexTest, VertexF_test)
{
    float f32 = 0.55f;
    f32++;

    constexpr Lut::CVertex<float> b(25.3f, 30.6f, 40.11f);

    const Lut::CVertex<int> ii(b);
    std::cout << "Vertex<float>: " << b << " Vertex after int cast: " << ii << std::endl;

    std::cout << "Vertex<float>: " << b << " Vertex after int cast: " << static_cast<Lut::CVertex<int>>(b) << std::endl;

    Lut::CVertex<int> integer;
    integer.floor();

    Lut::CVertex<float> e(b);

    e = e + 0.5f;
    e = 0.75f + e;

     //dummy, let's update it later
    EXPECT_EQ(1, 1);
}

TEST(VertexTest, VertexI_test)
{
    Lut::CVertex<int32_t> a;

    //dummy, let's update it later
    EXPECT_EQ(1, 1);
}
