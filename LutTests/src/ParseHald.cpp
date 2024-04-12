#include "gtest/gtest.h"
#include "lutHald.h"

const std::string dbgLutsFolder = { HALD_LUT_FOLDER };

TEST(Parse_Hald1 , help_option)
{
	const std::string lutName{ dbgLutsFolder + "/contrast.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
//	GTEST_EXPECT_TRUE(lutSize == 2);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}


/*
 add huge LUT with 96 entries - where we may take lut like this ??? 
*/

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	std::cout << "Parse from: " << dbgLutsFolder << std::endl;
	return RUN_ALL_TESTS();	
}
