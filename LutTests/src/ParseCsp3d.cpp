#include "gtest/gtest.h"
#include "lutCineSpace3D.h"

const std::string dbgLutsFolder = { CSP_LUT_FOLDER };


TEST (ParseCsp3d, Parse_CSP_3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/non-uniform.csp" };
	CCineSpaceLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
//	auto const lutSize = lutFileF32.getLutSize();
//	GTEST_EXPECT_TRUE(lutSize == 2);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (ParseCsp3d, Parse_CSP_3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/non-uniform.csp" };
	CCineSpaceLut3D<float> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
//	auto const lutSize = lutFileF32.getLutSize();
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
