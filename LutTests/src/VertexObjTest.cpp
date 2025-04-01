#include "gtest/gtest.h"
#include <iostream>
#include "Vertex.hpp"


TEST(VertexTest, VertexF_test)
{
    Lut::CVertex<float> a;

    Lut::CVertex<float> b(25.3f, 30.6f, 40.11f);

    Lut::CVertex<float> c = floor(b);
    Lut::CVertex<float> d = ceil (b);

    //dummy, let's update it later
    EXPECT_EQ(1, 1);
}

TEST(VertexTest, VertexI_test)
{
    Lut::CVertex<int32_t> a;

    //dummy, let's update it later
    EXPECT_EQ(1, 1);
}