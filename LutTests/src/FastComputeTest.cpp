#include "gtest/gtest.h"
#include <iostream>
#include "fast_compute.h"

TEST(FastFloorF32Test, f32_test1)
{
	const float f1 = 2.131f;
	const float f2 = 4.894f;
	const float f3 = 10.0f;
	const float f4 = -20.13f;
	const float f5 = 0.0f;
	
	const float floor_f1 = FastCompute::Floor(f1);
	const float floor_f2 = FastCompute::Floor(f2);
	const float floor_f3 = FastCompute::Floor(f3);
	const float floor_f4 = FastCompute::Floor(f4);
	const float floor_f5 = FastCompute::Floor(f5);

	const bool b1 = (2.f   == floor_f1);
	const bool b2 = (4.f   == floor_f2);
	const bool b3 = (10.f  == floor_f3);
	const bool b4 = (-21.f == floor_f4);
	const bool b5 = (0.0f  == floor_f5);
 	
        if (false == (b1 && b2 && b3 && b4 && b5))
	{
		std::cout << "floor (2.131f)  = " << floor_f1 << std::endl;	
		std::cout << "floor (4.894f)  = " << floor_f2 << std::endl;	
		std::cout << "floor (10.0f)   = " << floor_f3 << std::endl;	
		std::cout << "floor (-20.13f) = " << floor_f4 << std::endl;	
		std::cout << "floor (0.0f)    = " << floor_f5 << std::endl;	
	}

	EXPECT_EQ(true, b1 && b2 && b3 && b4 && b5);
	return;
}

TEST(FastFloorF64Test, f64_test1)
{
	const double f1 = 2.131;
	const double f2 = 4.894;
	const double f3 = 10.0;
	const double f4 = -20.13;
	const double f5 = 0.0;
	
	const double floor_f1 = FastCompute::Floor(f1);
	const double floor_f2 = FastCompute::Floor(f2);
	const double floor_f3 = FastCompute::Floor(f3);
	const double floor_f4 = FastCompute::Floor(f4);
	const double floor_f5 = FastCompute::Floor(f5);

	const bool b1 = (2.0   == floor_f1);
	const bool b2 = (4.0   == floor_f2);
	const bool b3 = (10.0  == floor_f3);
	const bool b4 = (-21.0 == floor_f4);
	const bool b5 = (0.00  == floor_f5);
 	
        if (false == (b1 && b2 && b3 && b4 && b5))
	{
		std::cout << "floor (2.131)  = " << floor_f1 << std::endl;	
		std::cout << "floor (4.894)  = " << floor_f2 << std::endl;	
		std::cout << "floor (10.0)   = " << floor_f3 << std::endl;	
		std::cout << "floor (-20.13) = " << floor_f4 << std::endl;	
		std::cout << "floor (0.0)    = " << floor_f5 << std::endl;	
	}

	EXPECT_EQ(true, b1 && b2 && b3 && b4 && b5);
	return;
}

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();	
}
