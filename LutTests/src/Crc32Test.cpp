#include "gtest/gtest.h"
#include <iostream>
#include "crc_utils.h"

/* https://lxp32.github.io/docs/a-simple-example-crc32-calculation/ */
TEST(crc32_test , help_option)
{
	const std::array<uint8_t, 9> TestArray { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	constexpr uint32_t expected_crc32 = 0xCBF43926u;
	const uint32_t computed_crc32 = crc32_reflected (TestArray);
	// std::cout << "Expected CRC32: " << std::hex << expected_crc32 << "  Computed CRC32: " << std::hex << computed_crc32 << std::endl;
	EXPECT_EQ(expected_crc32, computed_crc32);
	return;
}


int main (int argc, char** argv)
{
    	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();	
}
